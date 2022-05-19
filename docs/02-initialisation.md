# Initialisation

Designing how and what to track in your app is an important decision. Check out our docs about tracking design [here](https://docs.snowplowanalytics.com/docs/understanding-tracking-design/introduction-to-tracking-design/).

## Import the library

Import the C++ Tracker library and use the `snowplow` namespace like so:

```cpp
#include "snowplow/snowplow.hpp"

using namespace snowplow;
```

That’s it – you are now ready to initialize a tracker instance that you will use to track events. There are 3 ways to do that:

1. Create a tracker with default configuration using the `Snowplow` interface.
2. Create a tracker with custom configuration using the `Snowplow` interface.
3. Create and manage `Tracker`, `Emitter`, and `ClientSession` directly.

## Option 1: Creating a tracker with default configuration using the "Snowplow" interface

The `Snowplow` class provides static methods that let you easily create a new tracker. It can be as simple as:

```cpp
auto tracker = Snowplow::create_tracker(
    "ns", // tracker namespace used to identify the tracker
    "https://com.acme.collector", // Snowplow collector URL
    POST, // HTTP method used to send events to the collector
    "events.db" // Relative path to an SQLite database used for event queue and session tracking
);
```

Optionally, you may choose to attach a `Subject` instance with information about the user and device (see the next page on [Adding data](03-adding-data.md) to learn more about the `Subject`):

```cpp
auto subject = std::make_shared<Subject>() // initialize a C++ shared pointer for the Subject
subject->set_user_id("a-user-id");

auto tracker = Snowplow::create_tracker("ns", "https://com.acme.collector", POST, "events.db", subject);
```

Finally, client session tracking will be automatically enabled and session information will be attached to all events (see documentation on [Client sessions](06-client-sessions.md) for more details). If you wish to disable session tracking, pass `false` as the last argument:

```cpp
auto tracker = Snowplow::create_tracker("ns", "https://com.acme.collector", POST, "events.db", subject, false);
```

After you create a tracker, you can access it from anywhere using it's namespace:

```cpp
auto tracker = Snowplow::get_tracker("ns");
```

You can also access the default tracker using `Snowplow::get_default_tracker()`. The default tracker is the first initialized tracker (unless it is removed). If you have multiple trackers, you can choose the default tracker by calling `Snowplow::set_default_tracker(tracker2)`.

To remove reference of previously initialized tracker from the Snowplow interface (the tracker will be deleted once all remaininig references to it are deleted):

```cpp
Snowplow::remove_tracker(tracker);
```

## Option 2: Creating a tracker with custom configuration using the "Snowplow" interface

There are a number of settings that you may want to customize when creating a tracker instance. You can make use of configuration objects to provide custom configuration for the tracker:

```cpp
TrackerConfiguration tracker_config(
    "namespace", // tracker namespace
    "app-id", // application ID
    mob // platform that the tracker runs on
);
tracker_config.set_desktop_context(false); // do not attach a desktop context entity to events (on by default)

NetworkConfiguration network_config("https://com.acme.collector", POST); // collector URL and method

EmitterConfiguration emitter_config("sp.db"); // event queue DB path if SQLite or custom `EventStore` instance
emitter_config.set_batch_size(500); // maximum number of events to send at a time

SessionConfiguration session_config(
    "sp.db", // session DB path if SQLite (usually same as event queue) or custom `SessionStore` instance
    5000, // foreground timeout in ms
    5000 // background timeout in ms
);

auto tracker = Snowplow::create_tracker(tracker_config, network_config, emitter_config, session_config);
```

### Tracker configuration using "TrackerConfiguration"

`TrackerConfiguration` contains settings to identify and configure the tracker. It's constructor takes 3 attributes:

| Constructor attribute | Description | Default |
|---|---|---|
| `name_space` | Tracker namespace to identify the tracker and also attach as a property to tracked events. | None |
| `app_id` | Application ID. | "" |
| `platform` | Enum of the platform the Tracker is running on, can be one of: web, mob, pc, app, srv, tv, cnsl, iot | srv |

It further provides 2 optional setter functions:

| Setter | Description | Default |
|---|---|---|
| `set_use_base64` | Whether to use base64 encoding in events. | true |
| `set_desktop_context` | Whether to add a desktop_context, which gathers information about the device the tracker is running on, to each event. | true |

### Network configuration using "NetworkConfiguration"

`NetworkConfiguration` has only two properties set in it's constructor to configure the Snowplow collector:

| Constructor attribute | Description | Default |
|---|---|---|
| `collector_url` | Full URL of the Snowplow collector including the protocol (or defaults to HTTPS if protocol not present). | None |
| `method` | HTTP method to use when sending events to collector – GET or POST. | POST |
| `curl_cookie_file` | Path to a file where to store cookies in case http_client is nullptr and the CURL HTTP client is used – only relevant under Linux (CURL is not used under Windows and macOS) | In-memory cookie storage with CURL on Linux, platform native storage on Windows and macOS |

Additionally, it provides the following setter functions:

| Setter | Description | Default |
|---|---|---|
| `set_http_client` | Unique pointer to a custom HTTP client to send GET and POST requests with. | Platform-specific implementation. |

### Emitter configuration using "EmitterConfiguration"

`EmitterConfiguration` brings additional settings for the constructor. It provides two constructors that accept the event store either as a path to the SQLite database or a custom `EventStore` object. The following configurations are identical:

```cpp
EmitterConfiguration config1("sp.db");

auto storage = std::make_shared<SqliteStorage>("sp.db");
EmitterConfiguration config2(storage); // you can also pass a custom `EventStore`
```

Additionally, it provides the following setter functions:

| Setter | Description | Default |
|---|---|---|
| `set_batch_size` | The maximum amount of events to send at a time. | 250 events |
| `set_byte_limit_get` | The byte limit when sending a GET request. | 40000 bytes |
| `set_byte_limit_post` | The byte limit when sending a POST request. | 40000 bytes |
| `set_request_callback` | Set a callback to call after emit requests are made with the resulting emit status (see page about Emitter for more info). | None |
| `set_custom_retry_for_status_code` | Set a custom retry rule for when the HTTP status code is received in emit response from Collector (see page about Emitter for more details). | None |

### Session configuration using "SessionConfiguration"

`SessionConfiguration` is an optional attribute in `create_tracker`. If passed, session tracking will be enabled with the given configuration. If not passed, session tracking will not be enabled. The constructor takes 3 attributes:

| Constructor attribute | Description | Default |
|---|---|---|
| `session_store` or `db_name` | You may either pass a path to an SQLite database to be used for session storage or a custom implementation of `SessionStore` similar as for `EventStore` in `EmitterConfiguration`. | None |
| `foreground_timeout` | Timeout in ms for updating the session when the app is in background. | 30 minutes |
| `background_timeout` | Timeout in ms for updating the session when the app is in foreground. | 30 minutes |

## Option 3: Managing "Tracker", "Emitter", and "ClientSession" directly

The third option to initialise a new tracker is to instantiate it and the related components directly. This option is suitable in case you want supply a custom emitter or client session implementation. If you don't want to do that, we recommend using Option 2 which gives you the same configuration options with a simpler API.

The following example takes you through the steps needed to initialize a storage, emitter, subject, client session, and tracker:

```cpp
#include "snowplow/snowplow.hpp"

// 1. create storage for event queue and session information
auto storage = std::make_shared<SqliteStorage>("sp.db");
// 2. create an emitter for sending event to Snowplow collector
auto emitter = std::make_shared<Emitter>(storage, "com.acme.collector");
// 3. create a subject with user and device information
auto subject = std::make_shared<Subject>();
subject->set_user_id("a-user-id");
// 4. optionally create a client_session for session tracking
auto client_session = std::make_shared<ClientSession>(storage, 5000, 5000);
// 5. finally, create the tracker instance
auto tracker = std::make_shared<Tracker>(emitter, subject, client_session, "pc", "app_id", "ns");
```

Once you initialize the tracker, you can optionally register it with the Snowplow interface:

```cpp
Snowplow::register_tracker(tracker);
```

This will make accessible from anywhere using `Snowplow::get_tracker("ns")`.

The following will explain the individual components shown in the above code sample in more detail.

### Storage

The storage has two functions in the example above – it is used by the `emitter` to persist an event queue with events to be sent, and it is used by the `client_session` to persist the current session. The tracker provides an SQL storage (`SqliteStorage`) implementation, but you may introduce your own storage as described in ["Emitters"](05-emitters.md) and ["Client Sessions"](06-client-sessions.md).

### Emitter

Emitter is a required component responsible for sending tracked events to the collector.

Accepts an argument of an Emitter instance pointer; if the object is `NULL` will throw an exception. See Emitters for more on emitter configuration.

| **Argument Name** | **Description** | **Required?** | **Default** |
| --- | --- | --- | --- |
| `event_store` | Defines the database to use for event queue | Yes | – |
| `uri` | The collector URI (excluding protocol) to send events to | Yes | – |
| `method` | The request type to use (GET or POST) | No | POST |
| `protocol` | The protocol to use (HTTP or HTTPS) | No | HTTPS |
| `batch_size` | The maximum amount of events to send at a time | No | 250 events |
| `byte_limit_post` | The byte limit when sending a POST request | No | 40000 bytes |
| `byte_limit_get` | The byte limit when sending a GET request | No | 40000 bytes |
| `http_client` | Unique pointer to a custom HTTP client to send GET and POST requests with | No | Platform-specific implementation. |
| `curl_cookie_file` | Path to a file where to store cookies in case http_client is nullptr and the CURL HTTP client is used – only relevant under Linux (CURL is not used under Windows and macOS) | No | In-memory cookie storage with CURL on Linux, platform native storage on Windows and macOS |

### Subject

The user which the Tracker will track. Accepts an argument of a Subject instance pointer.

You don’t need to set this during Tracker construction; you can use the `tracker.set_subject(...)` method afterwards. In fact, you don’t need to create a subject at all. If you don’t, though, your events won’t contain user-specific data such as timezone and language.

### Client session

The client sessions which the Tracker will attach to each event. Accepts an argument of a ClientSession instance pointer.

Adds the ability to attach a ClientSession context to each event that leaves the Tracker. This object will persistently store information about the sessions that have occurred for the life of the application – unless the database is destroyed.

### Tracker

The tracker instance lets you track events. You should maintain a reference to the initialized tracker during the app lifetime.

| **Argument Name** | **Description** | **Required?** | **Default** |
| --- | --- | --- | --- |
| `emitter` | The emitter to which events are sent | Yes | – |
| `subject` | The user being tracked | No  | nullptr |
| `client_session` | Client session recording | No  | nullptr |
| `platform` | The platform the Tracker is running on | No  | "srv" |
| `app_id` | The application ID | No  | "" |
| `name_space` | The namespace of the tracker instance used to identify the tracker. | No  | "" |
| `use_base64` | Whether to enable [base 64 encoding](https://en.wikipedia.org/wiki/Base64). Defaults to true to ensure that no data is lost or corrupted. | No  | true |
| `desktop_context` | Whether to add a `desktop_context` to events | No  | true |

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
