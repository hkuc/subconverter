#include <csignal>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <sys/resource.h>

#include "handler/settings.h"
#include "handler/webget.h"
#include "utils/file_extra.h"

Settings global;

static void require(bool condition, const char *message)
{
    if(!condition)
        throw std::runtime_error(message);
}

int main(int argc, char **argv)
{
    try
    {
        require(argc == 4, "usage: download_driver MODE URL WORKDIR");
        std::filesystem::current_path(argv[3]);
        global.logLevel = LOG_LEVEL_FATAL;
        global.maxAllowedDownloadSize = 0;
        const std::string mode = argv[1], url = argv[2];
        const std::string expected = std::string(12000, 'x') + "END\n";
        const std::string cache = "cache/" + getMD5(url);

        if(mode == "retry" || mode == "api-retry")
        {
            global.APIMode = mode == "api-retry";
            int status = 0;
            std::string body, headers;
            FetchArgument argument {HTTP_GET, url, ""};
            FetchResult result {&status, &body, &headers};
            webGet(argument, result);
            require(body == expected, "retry returned missing or duplicated response bytes");
            require(headers.find("X-Attempt: 1") == std::string::npos,
                    "retry retained headers from the failed attempt");
        }
        else if(mode == "failed-cache")
        {
            require(webGet(url, "", 60).empty(), "incomplete download was returned as content");
            require(!fileExist(cache), "failed HTTP 200 download created a cache entry");
            require(webGet(url, "", 60) == expected, "failed download poisoned the next request");
            require(fileGet(cache) == expected, "successful response was not cached completely");
        }
        else if(mode == "fallback")
        {
            require(webGet(url, "", 60) == expected, "initial cache population failed");
            std::filesystem::last_write_time(cache,
                std::filesystem::file_time_type::clock::now() - std::chrono::seconds(120));
            global.serveCacheOnFetchFail = true;
            require(webGet(url, "", 60) == expected, "failed refresh did not fall back to old cache");
            require(fileGet(cache) == expected, "failed refresh overwrote the good cache");
        }
        else if(mode == "atomic-write")
        {
            require(fileWriteAtomic("existing", "previous") == 0, "initial atomic write failed");
            std::signal(SIGXFSZ, SIG_IGN);
            struct rlimit limit {8192, 8192};
            require(setrlimit(RLIMIT_FSIZE, &limit) == 0, "cannot set file-size test limit");
            require(fileWriteAtomic("existing", expected) != 0, "short file write was reported as success");
            require(fileGet("existing") == "previous", "short write replaced the previous cache");
            for(const auto &entry : std::filesystem::directory_iterator("."))
                require(entry.path().filename().string().find(".tmp.") == std::string::npos,
                        "failed atomic write left a temporary file");
        }
        else
            throw std::runtime_error("unknown test mode");
        return 0;
    }
    catch(const std::exception &error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
