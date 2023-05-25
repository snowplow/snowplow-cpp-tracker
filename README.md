# C++ Analytics for Snowplow

[![maintained]][tracker-classificiation] [![Build Status][travis-image]][travis] [![Release][release-image]][releases] [![License][license-image]][license]

Snowplow is a scalable open-source platform for rich, high quality, low-latency data collection. It is designed to collect high quality, complete behavioral data for enterprise business.

**To find out more, please check out the [Snowplow website][website] and our [documentation][docs].**

## Snowplow C++ Tracker Overview

Snowplow C++ tracker enables you to add analytics to your C++ applications, servers and games when using a [Snowplow][snowplow] pipeline.

## Quick Start

The tracker supports macOS, Windows, and Linux.

### Installation

There are three ways to install the tracker in your app:

1. By adding the project into your `CMakeLists.txt` as a subdirectory.
2. By installing the project and importing it into your app using CMake's `find_package` command.
3. By copying source files inside the `include` folder into your codebase.

#### As a subdirectory in your CMake project

CMake version 3.15 or greater is required. You may add the library to your project target (`your-target`) using `FetchContent` like so:

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

#### As an imported target in your CMake project

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

#### Copying files to your project

Download the most recent release from the [releases section](https://github.com/snowplow/snowplow-cpp-tracker/releases). Everything in the `include` folder will need to be included in your application.

The project has two dependencies that need to be included in your project: [nlohmann/json](https://github.com/nlohmann/json) and [the amalgamated version of sqlite3](https://www.sqlite.org/download.html). You will need to update the include paths in headers `include/snowplow/thirdparty/json.hpp` and `include/snowplow/thirdparty/sqlite3.hpp`.

#### Additional requirements under Linux

The following libraries need to be installed:

* curl (using `apt install libcurl4-openssl-dev` on Ubuntu)
* uuid (using `apt install uuid-dev` on Ubuntu)

### Using the tracker

Import using the `snowplow/snowplow.hpp` header file and initialize the tracker with your Snowplow collector endpoint and tracker configuration:

```cpp
#include "snowplow/snowplow.hpp"

using namespace snowplow;

// Initialize tracker with namespace, collector URI, HTTP method, and SQLite database path (see docs for other options)
auto tracker = Snowplow::create_tracker("ns", "https://collector.com", POST, "sp.db");
```

Track custom events (see the documentation for the full list of supported event types):

```cpp
// structured event
StructuredEvent se("category", "action");
tracker->track(se);

// screen view event
ScreenViewEvent sve;
string name = "Screen ID - 5asd56";
sve.name = &name;
tracker->track(sve);
```

Check the tracked events in a [Snowplow Micro](https://docs.snowplow.io/docs/understanding-your-pipeline/what-is-snowplow-micro/) or [Snowplow Mini](https://docs.snowplow.io/docs/understanding-your-pipeline/what-is-snowplow-mini/) instance.

## Find out more

| Technical Docs                  | API Docs               | Contributing                      |
|---------------------------------|---------------------------|-----------------------------------|
| [![i1][techdocs-image]][techdocs]          | [![i2][techdocs-image]][apidocs]       | [![i4][contributing-image]][contributing]        |
| **[Technical Docs][techdocs]** | **[API Docs][apidocs]** | **[Contributing][contributing]** |

## Maintainer Quick Start

### Building on macOS and Linux

```bash
 host> git clone https://github.com/snowplow/snowplow-cpp-tracker
 host> cd snowplow-cpp-tracker
 host> cmake -D SNOWPLOW_BUILD_TESTS=1 -D SNOWPLOW_BUILD_EXAMPLE=1 -B build .
 host> cd build
 host> make
```

This will create two executables - the first is the test suite. To run the test suite, first start a [Snowplow Micro](https://github.com/snowplow-incubator/snowplow-micro) instance and run:

```bash
 host> ./snowplow-tests
```

The other is an example program which will send one of every type of event to an endpoint of your choosing like so:

```bash
 host> ./examples/snowplow-example {{ your collector uri }}
```

If you make changes only to a header file there is a chance it won't be picked up by make in which case you will need to:

```bash
 host> make clean
 host> make
```

#### Performance testing

The project also provides performance tests to measure changes in performance of the tracker. The tests measure performance under a few scenarios in which they vary the emitter and session.

Build and run the performance using the following steps from the root of the project:

```bash
 host> cmake -D SNOWPLOW_BUILD_PERFORMANCE=1 -B build .
 host> cd build && make && cd ..
 host> ./build/snowplow-performance
```

To compare with historical performance measurements (logged in the `performance/logs.txt` file), run the following Python script that will output a table with the performance comparison:

```bash
 host> ./performance/stats.py                                       

 Metric                                | Max     | Min     | Mean    | Last    |
--------------------------------------------------------------------------------
 mocked emitter and mocked session     | 5.27s   | 5.08s   | 5.18s   | 5.27s   |
 mocked emitter and real session       | 5.08s   | 5.04s   | 5.06s   | 5.07s   |
 mute emitter and mocked session       | 18.77s  | 17.29s  | 18.08s  | 18.77s  |
 mute emitter and real session         | 28.02s  | 22.61s  | 25.06s  | 22.61s  |
```

### Building on Windows

Use the [CMake build tool](https://cmake.org/runningcmake/) to configure and generate the Visual Studio project files.
Build and run the project using the Visual Studio IDE.

## Copyright and license

The Snowplow C++ Tracker is copyright 2022 Snowplow Analytics Ltd.

Licensed under the **[Apache License, Version 2.0][license]** (the "License");
you may not use this software except in compliance with the License.

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

[website]: https://snowplow.io
[snowplow]: https://github.com/snowplow/snowplow
[docs]: https://docs.snowplow.io/

[travis-image]: https://travis-ci.org/snowplow/snowplow-cpp-tracker.png?branch=master
[travis]: https://travis-ci.org/snowplow/snowplow-cpp-tracker

[release-image]: https://img.shields.io/badge/release-1.0.0-6ad7e5.svg?style=flat
[releases]: https://github.com/snowplow/snowplow-cpp-tracker/releases

[license-image]: https://img.shields.io/badge/license-Apache--2-blue.svg?style=flat
[license]: https://www.apache.org/licenses/LICENSE-2.0

[techdocs-image]: https://d3i6fms1cm1j0i.cloudfront.net/github/images/techdocs.png
[roadmap-image]: https://d3i6fms1cm1j0i.cloudfront.net/github/images/roadmap.png
[contributing-image]: https://d3i6fms1cm1j0i.cloudfront.net/github/images/contributing.png

[techdocs]: https://docs.snowplow.io/docs/collecting-data/collecting-from-own-applications/c-tracker/
[roadmap]: https://github.com/snowplow/snowplow/projects/7
[contributing]: https://github.com/snowplow/snowplow-cpp-tracker/blob/master/CONTRIBUTING.md
[apidocs]: https://snowplow.github.io/snowplow-cpp-tracker

[tracker-classificiation]: https://docs.snowplow.io/docs/collecting-data/collecting-from-own-applications/tracker-maintenance-classification/
[maintained]: https://img.shields.io/static/v1?style=flat&label=Snowplow&message=Maintained&color=9e62dd&labelColor=9ba0aa&logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAMAAAAoLQ9TAAAAeFBMVEVMaXGXANeYANeXANZbAJmXANeUANSQAM+XANeMAMpaAJhZAJeZANiXANaXANaOAM2WANVnAKWXANZ9ALtmAKVaAJmXANZaAJlXAJZdAJxaAJlZAJdbAJlbAJmQAM+UANKZANhhAJ+EAL+BAL9oAKZnAKVjAKF1ALNBd8J1AAAAKHRSTlMAa1hWXyteBTQJIEwRgUh2JjJon21wcBgNfmc+JlOBQjwezWF2l5dXzkW3/wAAAHpJREFUeNokhQOCA1EAxTL85hi7dXv/E5YPCYBq5DeN4pcqV1XbtW/xTVMIMAZE0cBHEaZhBmIQwCFofeprPUHqjmD/+7peztd62dWQRkvrQayXkn01f/gWp2CrxfjY7rcZ5V7DEMDQgmEozFpZqLUYDsNwOqbnMLwPAJEwCopZxKttAAAAAElFTkSuQmCC
