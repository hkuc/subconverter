cmake_minimum_required(VERSION 3.13)
project(download_regression LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
find_package(CURL REQUIRED)
find_package(yaml-cpp REQUIRED)
set(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(WEBGET_SOURCE "${PROJECT_ROOT}/src/handler/webget.cpp" CACHE FILEPATH "Downloader under test")
add_executable(download_driver
    download_driver.cpp
    ${WEBGET_SOURCE}
    ${PROJECT_ROOT}/src/utils/file.cpp
    ${PROJECT_ROOT}/src/utils/string.cpp
    ${PROJECT_ROOT}/src/utils/base64/base64.cpp
    ${PROJECT_ROOT}/src/utils/md5/md5.cpp
    ${PROJECT_ROOT}/src/utils/urlencode.cpp
    ${PROJECT_ROOT}/src/utils/logger.cpp)
target_compile_definitions(download_driver PRIVATE NO_JS_RUNTIME)
target_include_directories(download_driver PRIVATE ${PROJECT_ROOT}/src ${PROJECT_ROOT}/src/handler ${PROJECT_ROOT}/include)
target_link_libraries(download_driver PRIVATE CURL::libcurl yaml-cpp)
enable_testing()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_test(NAME download_regression COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/test_downloads.py $<TARGET_FILE:download_driver>)
