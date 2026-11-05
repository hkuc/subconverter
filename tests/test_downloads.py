"""Exercise the production downloader against deliberately interrupted HTTP responses."""
import collections
import http.server
import pathlib
import subprocess
import sys
import tempfile
import threading
import unittest

DRIVER = pathlib.Path(sys.argv.pop(1)).resolve()
BODY = b"x" * 12000 + b"END\n"


class Handler(http.server.BaseHTTPRequestHandler):
    counts = collections.Counter()

    def do_GET(self):
        self.counts[self.path] += 1
        attempt = self.counts[self.path]
        mode = self.path.strip("/")
        partial = (mode in ("retry", "api-retry") and attempt == 1
                   or mode == "failed-cache" and attempt <= 2
                   or mode == "fallback" and attempt > 1)
        self.send_response(200)
        self.send_header("Content-Length", str(len(BODY)))
        self.send_header("X-Attempt", str(attempt))
        self.end_headers()
        self.wfile.write(BODY[:8192] if partial else BODY)
        self.wfile.flush()
        self.close_connection = True

    def log_message(self, *_):
        pass


class DownloadTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), Handler)
        cls.thread = threading.Thread(target=cls.server.serve_forever, daemon=True)
        cls.thread.start()

    @classmethod
    def tearDownClass(cls):
        cls.server.shutdown()
        cls.server.server_close()
        cls.thread.join()

    def check_mode(self, mode):
        url = f"http://127.0.0.1:{self.server.server_port}/{mode}"
        with tempfile.TemporaryDirectory() as work:
            result = subprocess.run([str(DRIVER), mode, url, work],
                                    capture_output=True, text=True, timeout=15)
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_retry_discards_partial_body_and_headers(self):
        self.check_mode("retry")

    def test_api_mode_retries_interrupted_get(self):
        self.check_mode("api-retry")

    def test_failed_download_never_poison_cache(self):
        self.check_mode("failed-cache")

    def test_failed_refresh_preserves_good_cache(self):
        self.check_mode("fallback")

    def test_short_disk_write_keeps_previous_file(self):
        self.check_mode("atomic-write")


if __name__ == "__main__":
    unittest.main()
