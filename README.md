# C++ Analytics for Snowplow

[![early-release]][tracker-classificiation] [![Build Status][travis-image]][travis] [![Release][release-image]][releases] [![License][license-image]][license]

Snowplow is a scalable open-source platform for rich, high quality, low-latency data collection. It is designed to collect high quality, complete behavioral data for enterprise business.

**To find out more, please check out the [Snowplow website][website] and our [documentation][docs].**

## Snowplow C++ Tracker Overview

Snowplow C++ tracker enables you to add analytics to your C++ applications, servers and games when using a [Snowplow][snowplow] pipeline.

## Quick Start

The tracker supports macOS, Windows, and Linux.

### Installation

There are two ways to install the tracker in your app:

1. By including the project using cmake.
2. By copying source files inside the `include` folder into your codebase.

#### Using cmake

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

Check the tracked events in a [Snowplow Micro](https://docs.snowplowanalytics.com/docs/understanding-your-pipeline/what-is-snowplow-micro/) or [Snowplow Mini](https://docs.snowplowanalytics.com/docs/understanding-your-pipeline/what-is-snowplow-mini/) instance.

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
 host> ./snowplow-example {{ your collector uri }}
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

 ```git clone https://github.com/snowplow/snowplow-cpp-tracker```

In the cloned directory two [Visual Studio 2015](https://www.visualstudio.com/en-us/products/visual-studio-community-vs.aspx) project files are available:

* `snowplow-cpp-tracker.sln`
    This is required to compile the tracker and run the tests (you'll need this one to edit the tracker itself)
* `snowplow-cpp-tracker-example.sln`
    This is a demo project showing use of this tracker

To run the tests under windows, you need to open the `snowplow-cpp-tracker.sln` solution, build it for your target platform *and run the resulting executable*.

## Copyright and license

The Snowplow C++ Tracker is copyright 2022 Snowplow Analytics Ltd.

Licensed under the **[Apache License, Version 2.0][license]** (the "License");
you may not use this software except in compliance with the License.

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

[website]: https://snowplowanalytics.com
[snowplow]: https://github.com/snowplow/snowplow
[docs]: https://docs.snowplowanalytics.com/

[travis-image]: https://travis-ci.org/snowplow/snowplow-cpp-tracker.png?branch=master
[travis]: https://travis-ci.org/snowplow/snowplow-cpp-tracker

[release-image]: https://img.shields.io/badge/release-0.4.0-6ad7e5.svg?style=flat
[releases]: https://github.com/snowplow/snowplow-cpp-tracker/releases

[license-image]: https://img.shields.io/badge/license-Apache--2-blue.svg?style=flat
[license]: https://www.apache.org/licenses/LICENSE-2.0

[techdocs-image]: https://d3i6fms1cm1j0i.cloudfront.net/github/images/techdocs.png
[roadmap-image]: https://d3i6fms1cm1j0i.cloudfront.net/github/images/roadmap.png
[contributing-image]: https://d3i6fms1cm1j0i.cloudfront.net/github/images/contributing.png

[techdocs]: https://docs.snowplowanalytics.com/docs/collecting-data/collecting-from-own-applications/c-tracker/
[roadmap]: https://github.com/snowplow/snowplow/projects/7
[contributing]: https://github.com/snowplow/snowplow-cpp-tracker/blob/master/CONTRIBUTING.md
[apidocs]: https://snowplow.github.io/snowplow-cpp-tracker

[tracker-classificiation]: https://github.com/snowplow/snowplow/wiki/Tracker-Maintenance-Classification
[early-release]: https://img.shields.io/static/v1?style=flat&label=Snowplow&message=Early%20Release&color=014477&labelColor=9ba0aa&logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAMAAAAoLQ9TAAAAeFBMVEVMaXGXANeYANeXANZbAJmXANeUANSQAM+XANeMAMpaAJhZAJeZANiXANaXANaOAM2WANVnAKWXANZ9ALtmAKVaAJmXANZaAJlXAJZdAJxaAJlZAJdbAJlbAJmQAM+UANKZANhhAJ+EAL+BAL9oAKZnAKVjAKF1ALNBd8J1AAAAKHRSTlMAa1hWXyteBTQJIEwRgUh2JjJon21wcBgNfmc+JlOBQjwezWF2l5dXzkW3/wAAAHpJREFUeNokhQOCA1EAxTL85hi7dXv/E5YPCYBq5DeN4pcqV1XbtW/xTVMIMAZE0cBHEaZhBmIQwCFofeprPUHqjmD/+7peztd62dWQRkvrQayXkn01f/gWp2CrxfjY7rcZ5V7DEMDQgmEozFpZqLUYDsNwOqbnMLwPAJEwCopZxKttAAAAAElFTkSuQmCC 
