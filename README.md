# C++ web analytics for Snowplow

[![early-release]][tracker-classificiation] [![Build Status][travis-image]][travis] [![Coverage Status][coverage-image]][coverage] [![Release][release-image]][releases] [![License][license-image]][license]

Snowplow is a scalable open-source platform for rich, high quality, low-latency data collection. It is designed to collect high quality, complete behavioral data for enterprise business.

**To find out more, please check out the [Snowplow website][website] and our [documentation][docs].**

## Snowplow C++ Tracker Overview

Snowplow C++ tracker enables you to add analytics to your C++ applications, servers and games when using a [Snowplow][snowplow] pipeline.

## Quick Start

The tracker supports macOS, Windows, and Linux.

### Installation

Download the most recent release from the [releases section](https://github.com/snowplow/snowplow-cpp-tracker/releases). Everything in both the `src` and `include` folders will need to be included in your application. It is important to keep the same folder structure as references to the included headers have been done like so: `../include/json.hpp`.

#### Requirements under Linux

The following libraries need to be installed:

* curl (using `apt install libcurl4-openssl-dev` on Ubuntu)
* uuid (using `apt install uuid-dev` on Ubuntu)

### Using the tracker

Import using the `snowplow.hpp` header file and initialize the tracker with your Snowplow collector endpoint and tracker configuration:

```cpp
#include "snowplow.hpp"

using namespace snowplow;

// Storage for events to be sent and current session
auto storage = std::make_shared<SqliteStorage>("sp.db");
// Emitter is responsible for sending events to a Snowplow Collector
Emitter emitter("com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 52000, storage);
// Subject defines additional information about your application's environment and user
Subject subject;
subject.set_user_id("a-user-id");
// Client session keeps track of user sessions
ClientSession client_session(storage, 5000, 5000);

string platform = "pc"; // platform the tracker is running on
string app_id = "openage"; // application ID
string name_space = "sp-pc"; // the name of the tracker instance
bool base64 = false; // whether to enable base 64 encoding
bool desktop_context = true; // add a context entity to events with information about the device

Tracker *tracker = Tracker::init(emitter, &subject, &client_session, &platform, &app_id, &name_space, &base64, &desktop_context);
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

## Developer Quick Start

### Building on macOS and Linux

```bash
 host> git clone https://github.com/snowplow/snowplow-cpp-tracker
 host> cd snowplow-cpp-tracker
 host> make
```

This will create two executables - the first is the test-suite which can be executed with `make unit-tests`.

The other is an example program which will send one of every type of event to an endpoint of your choosing like so:

```bash
 host> cd build
 host> ./tracker_example {{ your collector uri }}
```

If you make changes only to a header file there is a chance it won't be picked up by make in which case you will need to:

```bash
 host> make clean
 host> make
```

To run the test suite:

```bash
 host> make unit-tests
```

If you wish to generate a local code coverage report you will first need to install [lcov](http://ltp.sourceforge.net/coverage/lcov.php) on your host machine.  The easiest way to do this is using [brew](http://brew.sh/) under macOS:

```bash
 host> brew install lcov 
```

Then run the following:

```bash
 host> make lcov-genhtml
```

The above runs the test suite and then generates a full code coverage report which can be accessed by opening the `index.html` in the `build` directory.

#### Performance testing

The project also provides performance tests to measure changes in performance of the tracker. The tests measure performance under a few scenarios in which they vary the emitter and session.

To run performance tests on your machine:

```bash
 host> ./build/performance/tracker_performance
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

## Find out more

| Technical Docs                  | Setup Guide               | Roadmap                 | Contributing                      |
|---------------------------------|---------------------------|-------------------------|-----------------------------------|
| ![i1][techdocs-image]          | ![i2][setup-image]       | ![i3][roadmap-image]   | ![i4][contributing-image]        |
| **[Technical Docs][techdocs]** | **[Setup Guide][setup]** | **[Roadmap][roadmap]** | **[Contributing][contributing]** |

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

[release-image]: https://img.shields.io/badge/release-0.2.0-6ad7e5.svg?style=flat
[releases]: https://github.com/snowplow/snowplow-cpp-tracker/releases

[license-image]: https://img.shields.io/badge/license-Apache--2-blue.svg?style=flat
[license]: https://www.apache.org/licenses/LICENSE-2.0

[coverage-image]: https://coveralls.io/repos/github/snowplow/snowplow-cpp-tracker/badge.svg?branch=master
[coverage]: https://coveralls.io/github/snowplow/snowplow-cpp-tracker?branch=master

[techdocs-image]: https://d3i6fms1cm1j0i.cloudfront.net/github/images/techdocs.png
[setup-image]: https://d3i6fms1cm1j0i.cloudfront.net/github/images/setup.png
[roadmap-image]: https://d3i6fms1cm1j0i.cloudfront.net/github/images/roadmap.png
[contributing-image]: https://d3i6fms1cm1j0i.cloudfront.net/github/images/contributing.png

[techdocs]: https://docs.snowplowanalytics.com/docs/collecting-data/collecting-from-own-applications/c-tracker/
[setup]: https://docs.snowplowanalytics.com/docs/collecting-data/collecting-from-own-applications/c-tracker/setup/
[roadmap]: https://github.com/snowplow/snowplow/projects/7
[contributing]: https://github.com/snowplow/snowplow-cpp-tracker/blob/master/CONTRIBUTING.md

[tracker-classificiation]: https://github.com/snowplow/snowplow/wiki/Tracker-Maintenance-Classification
[early-release]: https://img.shields.io/static/v1?style=flat&label=Snowplow&message=Early%20Release&color=014477&labelColor=9ba0aa&logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAMAAAAoLQ9TAAAAeFBMVEVMaXGXANeYANeXANZbAJmXANeUANSQAM+XANeMAMpaAJhZAJeZANiXANaXANaOAM2WANVnAKWXANZ9ALtmAKVaAJmXANZaAJlXAJZdAJxaAJlZAJdbAJlbAJmQAM+UANKZANhhAJ+EAL+BAL9oAKZnAKVjAKF1ALNBd8J1AAAAKHRSTlMAa1hWXyteBTQJIEwRgUh2JjJon21wcBgNfmc+JlOBQjwezWF2l5dXzkW3/wAAAHpJREFUeNokhQOCA1EAxTL85hi7dXv/E5YPCYBq5DeN4pcqV1XbtW/xTVMIMAZE0cBHEaZhBmIQwCFofeprPUHqjmD/+7peztd62dWQRkvrQayXkn01f/gWp2CrxfjY7rcZ5V7DEMDQgmEozFpZqLUYDsNwOqbnMLwPAJEwCopZxKttAAAAAElFTkSuQmCC 
