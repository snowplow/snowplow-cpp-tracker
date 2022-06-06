# Setup

## Tracker compatibility

The Snowplow C++ Tracker has been built and tested using C++ 11 as a minimum.

Supported operating systems:

* macOS
* Windows
* Linux

## Installing in your project

There are two ways to install the tracker in your app:

1. By including the project using cmake.
2. By copying source files inside the `include` folder into your codebase.

### Using cmake

Cmake version 3.14 or greater is required. You may add the library to your project target (`your-target`) using `FetchContent` like so:

```cmake
include(FetchContent)
FetchContent_Declare(
    snowplow
    GIT_REPOSITORY https://github.com/snowplow/snowplow-cpp-tracker
    GIT_TAG        1.0.0
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
