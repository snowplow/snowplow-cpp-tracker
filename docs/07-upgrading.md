# Upgrading to Newer Versions

This page gives instructions for upgrading to newer versions of the C++ tracker.

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
