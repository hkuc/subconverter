# Download and cache regression tests

These tests compile the production downloader and file writer. A local HTTP
server advertises a complete response but closes the connection after 8 KiB.
The tests cover retry isolation, retries in API mode, failed cache population,
fallback to a previous cache, and disk writes stopped at 8 KiB.

On Linux with a C++20 compiler, CMake, Python 3, libcurl, yaml-cpp and RapidJSON:

```sh
cmake -S tests -B build/download-tests -DCMAKE_BUILD_TYPE=Debug
cmake --build build/download-tests -j2
ctest --test-dir build/download-tests --output-on-failure
```

The reproducible source image also runs these tests before compiling the server:

```sh
docker build -f scripts/Dockerfile.source -t subconverter:cachefix .
```

Existing damaged cache entries must be removed or replaced once after deploying
the fix. Changing the client or its INI template does not repair cached input.


The image starts with `-f /base/pref.toml`. For an existing deployment, bind its
runtime directory to `/base` so settings and the cache survive container
replacement. Back up that directory and the original image before switching.
The binary lives outside the bind mount at `/usr/local/bin/subconverter`.
