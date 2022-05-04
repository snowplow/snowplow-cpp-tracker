# Initialisation

Designing how and what to track in your app is an important decision. Check out our docs about tracking design [here](https://docs.snowplowanalytics.com/docs/understanding-tracking-design/introduction-to-tracking-design/).

## Import the library

Import the C++ Tracker library and use the `snowplow` namespace like so:

```cpp
#include "snowplow.hpp"

using namespace snowplow;
```

That’s it – you are now ready to initialize a tracker instance.

## Creating a tracker

You will first need to create an Emitter to which events will be sent and pass it a Storage where it will persist an event queue with unsent events:

```cpp
#include "snowplow.hpp"

using namespace snowplow;

auto storage = std::make_shared<SqliteStorage>("events.db");
Emitter emitter("com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 52000, storage);
Tracker *t = Tracker::init(emitter, NULL, NULL, NULL, NULL, NULL, NULL);
```

The Tracker can now be used. It is created as a static singleton object so as to avoid conflicts between multiple Trackers reading and writing to a single database. You can access a pointer to the Tracker by either the `Tracker::init(...)` function or by the `Tracker::instance()` function. To ensure the Tracker is cleaned up properly you will need to run `Tracker::close()` in your main destructor.

**NOTE**: If you attempt to access the `instance` without having first run `init` an exception will be thrown.

The optional tracker arguments:

| **Argument Name** | **Description** | **Required?** | **Default** |
| --- | --- | --- | --- |
| `emitter` | The emitter to which events are sent | Yes | `NULL` |
| `subject` | The user being tracked | No  | `NULL` |
| `client_session` | Client session recording | No  | `NULL` |
| `platform` | The platform the Tracker is running on | No  | `srv` |
| `app_id` | The application ID | No  | “   |
| `name_space` | The name of the tracker instance | No  | “   |
| `use_base64` | Whether to enable [base 64 encoding](https://en.wikipedia.org/wiki/Base64) | No  | `true` |
| `desktop_context` | Whether to add a `desktop_context` to events | No  | `true` |

A more complete example:

```cpp
#include "snowplow.hpp"

auto storage = std::make_shared<SqliteStorage>("sp.db"); // storage for events and sessions, see "Emitters" and "Client Sessions" to learn more

Emitter emitter("com.acme.collector", Emitter::Method::GET, Emitter::Protocol::HTTPS, 500, 52000, 52000, storage);

Subject subject;
subject.set_user_id("a-user-id");
subject.set_screen_resolution(1920, 1080);
subject.set_viewport(1080, 1080);
subject.set_color_depth(32);
subject.set_timezone("GMT");
subject.set_language("EN");

ClientSession client_session(storage, 5000, 5000);
string platform = "pc";
string app_id = "openage";
string name_space = "sp-pc";
bool base64 = false;
bool desktop_context = true;

Tracker *t = Tracker::init(emitter, &subject, &client_session, &platform, &app_id, &name_space, &base64, &desktop_context);
```

**NOTE**: To ensure all of your events are sent before closing your application you can call the tracker flush function which is a blocking call that will send everything in the database and then will join the daemon thread to the calling thread.

### "storage"

The storage has two functions in the example above – it is used by the `emitter` to persist an event queue with events to be sent, and it is used by the `client_session` to persist the current session. The tracker provides an SQL storage implementation, but you may introduce your own storage as described in "Emitters" and "Client Sessions".

### "emitter"

Accepts an argument of an Emitter instance pointer; if the object is `NULL` will throw an exception. See Emitters for more on emitter configuration.

### "subject"

The user which the Tracker will track. Accepts an argument of a Subject instance pointer.

You don’t need to set this during Tracker construction; you can use the `tracker.set_subject(...)` method afterwards. In fact, you don’t need to create a subject at all. If you don’t, though, your events won’t contain user-specific data such as timezone and language.

### "client_session"

The client sessions which the Tracker will attach to each event. Accepts an argument of a ClientSession instance pointer.

Adds the ability to attach a ClientSession context to each event that leaves the Tracker. This object will persistently store information about the sessions that have occurred for the life of the application – unless the database is destroyed.

### "platform"

By default we assume the Tracker will be running in a server environment. To override this provide your own platform string.

### "app_id"

The `app_id` argument lets you set the application ID to any string.

### "name_space"

If provided, the `name_space` argument will be attached to every event fired by the new tracker. This allows you to later identify which tracker fired which event if you have multiple trackers running.

### "use_base64"

By default, unstructured events and custom contexts are encoded using Base64 to ensure that no data is lost or corrupted.

### "desktop_context"

The `desktop_context` gathers extra information about the device it is running on and sends it along with every event that is made by the Tracker.

An example of the data in this context:

```json
{
    "deviceManufacturer": "Apple Inc.",
    "deviceModel": "MacPro3,1",
    "deviceProcessorCount": 8,
    "osIs64Bit": true,
    "osServicePack": "",
    "osType": "macOS",
    "osVersion": "10.11.2"
}
```

For more information the raw JsonSchema can be found [here](https://raw.githubusercontent.com/snowplow/iglu-central/master/schemas/com.snowplowanalytics.snowplow/desktop_context/jsonschema/1-0-0).
