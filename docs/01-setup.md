# Setup

## Tracker compatibility

The Snowplow C++ Tracker has been built and tested using C++ 11 as a minimum.

Supported operating systems:

* macOS
* Windows
* Linux

## Installing in your project

There are three ways to install the tracker in your app:

1. By adding the project into your `CMakeLists.txt` as a subdirectory.
2. By installing the project and importing it into your app using CMake's `find_package` command.
3. By copying source files inside the `include` folder into your codebase.

### As an imported target in your CMake project

First, build and install the project. Make sure the project uses the external JSON libraries (`SNOWPLOW_USE_EXTERNAL_JSON=ON`). If you're building a static library (`SNOWPLOW_USE_EXTERNAL_SQLITE=ON`) you also need to use SQLite3 as an external library (`SNOWPLOW_USE_EXTERNAL_SQLITE=ON`).

If you have `SQLite3`, `CURL` or `LibUUID` available as system libraries but you need to use them from a different package (e.g. from Conan) you need to set `CMAKE_FIND_PACKAGE_PREFER_CONFIG=ON` to prevent linking to the system libraries.

```cmake
cmake [...] -DCMAKE_INSTALL_PREFIX=[...]
    -DSNOWPLOW_USE_EXTERNAL_JSON=ON -DSNOWPLOW_USE_EXTERNAL_SQLITE=ON \
    -DCMAKE_FIND_PACKAGE_PREFER_CONFIG=ON \
    -DSNOWPLOW_BUILD_TESTS=0 -DSNOWPLOW_BUILD_EXAMPLE=0 -DSNOWPLOW_BUILD_PERFORMANCE=0
```

After building and installing the project you can use `find_package` to import it into your `CMakeLists.txt`:

```cmake
find_package(snowplow REQUIRED CONFIG)
...
target_link_libraries(your-target snowplow::snowplow)
```

Make sure your project finds the same dependencies what was visible for Snowplow when you were building and installing it. For example, if you have both system and local SQlite3 installations and `CMAKE_FIND_PACKAGE_PREFER_CONFIG` was `ON` for Snowplow but `OFF` for your project, Snowplow will be built with the local SQLite3 while during `find_package(snowplow)` in your project it will find the system one.

See the [example app](https://github.com/snowplow/snowplow-cpp-tracker/tree/master/examples) for instructions how to build and install the tracker using this option.

### As a subdirectory in your CMake project

Cmake version 3.15 or greater is required. You may add the library to your project target (`your-target`) using `FetchContent` like so:

```cmake
include(FetchContent)
FetchContent_Declare(
    snowplow
    GIT_REPOSITORY https://github.com/snowplow/snowplow-cpp-tracker
    GIT_TAG        2.0.0
)
FetchContent_MakeAvailable(snowplow)
target_link_libraries(your-target snowplow)
```

### Copying files to your project

Download the most recent release from the [releases section](https://github.com/snowplow/snowplow-cpp-tracker/releases). Everything in the `include` folder will need to be included in your application.

The project has two dependencies that need to be included in your project: [nlohmann/json](https://github.com/nlohmann/json) and [the amalgamated version of sqlite3](https://www.sqlite.org/download.html). You will need to update the include paths in headers `include/snowplow/thirdparty/json.hpp` and `include/snowplow/thirdparty/sqlite3.hpp`.

### Additional requirements under Linux

Additionally, under Linux, the following libraries need to be installed:

* curl (using `apt install libcurl4-openssl-dev` on Ubuntu)
* uuid (using `apt install uuid-dev` on Ubuntu)
