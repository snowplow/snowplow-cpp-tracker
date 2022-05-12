# Setup

## Tracker compatibility

The Snowplow C++ Tracker has been built and tested using C++ 11 as a minimum.

Supported operating systems:

* macOS
* Windows
* Linux

## Setup

The Tracker is hosted on Github and versions of the Tracker will need to be included by manually dragging and dropping the source files directly into your codebase.

Everything in both the `src` and `include` folders will need to be included in your application. It is important to keep the same folder structure as references to the included headers have been done like so: `../include/json.hpp`.

If you wish to change this you will need to manually update the `src` header files.

**NOTE**: We have included the raw `sqlite3.h & c` files which will need to be compiled separately before being linked to your main C++ project.
