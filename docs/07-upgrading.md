# Upgrading to Newer Versions

This page gives instructions for upgrading to newer versions of the C++ tracker.

## Upgrading to v2.0.0

There have been the following breaking changes:

* CMake minimum version changed 3.14 -> 3.15
* libcurl has been removed as a dependency of Snowplow on macOS and the CURL HTTP client is no longer available on macOS. If a project that is using Snowplow as a subdirectory relies on getting libcurl through Snowplow then it will fail now.
* Also, sqlite3, libcurl, libuuid are now private dependencies. If a project relies on getting the sqlite3/libcurl/libuuid include paths through the Snowplow tracker library, then it will fail now.

## Upgrading to v1.0.0

The package added support for the cmake build system. You may make use of this and import the package in your cmake build configuration. Importing source code by copying files is still supported. However, please note that the code was moved from the `src` folder to the `include` folder and you will need to import the sqlite3 and nlohmann/json dependencies on your own and update their location in `include/snowplow/thirdparty/sqlite3.hpp` and `include/snowplow/thirdparty/json.hpp` files. See instructions on [the Setup page](01-setup.md) for more information.

This version changed how the `Tracker` and `Emitter` instances are initialized and introduced the `Snowplow::create_tracker()` API. Please refer to the [page about initialization](02-initialisation.md) to learn more about the new API. The `Tracker::init()` calls should be replaced with `Snowplow::create_tracker()` and `Tracker::instance()` with `Snowplow::get_tracker(namespace)` or `Snowplow::get_default_tracker()`.

Also note that the default protocol has changed from HTTP to HTTPS.

## Upgrading to v0.3.0

There are a few breaking changes in this release. Please make sure to:

1. Remove the `Tracker::` prefix when referring to event types, e.g., use `ScreenViewEvent()` instead of `Tracker::ScreenViewEvent()`.
2. Use the common `tracker->track(event)` function to track events (instead of `tracker->track_self_describing_event(event)`, `tracker->track_screen_view(event)`, ...).
3. Use the `event.set_context(context)` and `event.set_true_timestamp(tt)` setters instead of accessing `event.contexts` and `event.true_timestamp` directly.
4. Event IDs are no longer accessible from the event objects but are returned from the `tracker->track(event)` function.
5. `Emitter` and `ClientSession` no longer accept database path string for storage but require an instance of `SqliteStorage` (or other storage implementation, see the docs).
6. Use `#include "snowplow.hpp"` to import all public APIs of the tracker instead of including individual files.

## Upgrading to v0.2.0

This version adds a `snowplow` namespace to all tracker components. You will need to import specific or all types from the namespace:

```cpp
using namespace snowplow;
```

The constructor for `ClientSession` also changed, dropping the last argument for `check_interval` that is no longer necessary. Simply remove the last argument when creating an instance of `ClientSession`.
