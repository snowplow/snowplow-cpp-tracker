# C++ web analytics for Snowplow

[![Build Status][travis-image]][travis] [![Release][release-image]][releases] [![License][license-image]][license]

## Overview

Snowplow event tracker for C++. Add analytics to your C++ applications, servers and games

## Developer Quickstart

### Building on Mac OSX

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

### Building on Windows

__ED TO ADD__

## Find out more

| Technical Docs                  | Setup Guide               | Roadmap                 | Contributing                      |
|---------------------------------|---------------------------|-------------------------|-----------------------------------|
| ![i1] [techdocs-image]          | ![i2] [setup-image]       | ![i3] [roadmap-image]   | ![i4] [contributing-image]        |
| **[Technical Docs] [techdocs]** | **[Setup Guide] [setup]** | **[Roadmap] [roadmap]** | **[Contributing] [contributing]** |

## Copyright and license

The Snowplow C++ Tracker is copyright 2016 Snowplow Analytics Ltd.

Licensed under the **[Apache License, Version 2.0] [license]** (the "License");
you may not use this software except in compliance with the License.

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

[travis-image]: https://travis-ci.org/snowplow/snowplow-cpp-tracker.png?branch=master
[travis]: https://travis-ci.org/snowplow/snowplow-cpp-tracker

[release-image]: http://img.shields.io/badge/release-0.1.0-6ad7e5.svg?style=flat
[releases]: https://github.com/snowplow/snowplow-golang-tracker/releases

[license-image]: http://img.shields.io/badge/license-Apache--2-blue.svg?style=flat
[license]: http://www.apache.org/licenses/LICENSE-2.0

[techdocs-image]: https://d3i6fms1cm1j0i.cloudfront.net/github/images/techdocs.png
[setup-image]: https://d3i6fms1cm1j0i.cloudfront.net/github/images/setup.png
[roadmap-image]: https://d3i6fms1cm1j0i.cloudfront.net/github/images/roadmap.png
[contributing-image]: https://d3i6fms1cm1j0i.cloudfront.net/github/images/contributing.png

[techdocs]: https://github.com/snowplow/snowplow/wiki/CPP-Tracker
[setup]: https://github.com/snowplow/snowplow/wiki/CPP-Tracker-Setup
[roadmap]: https://github.com/snowplow/snowplow/wiki/Product-roadmap
[contributing]: https://github.com/snowplow/snowplow/wiki/Contributing
