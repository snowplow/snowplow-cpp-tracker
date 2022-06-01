# Client Sessions

You can optionally decide to include Client Sessionization. This object will keep track of your users sessions and can be configured to timeout after a certain amount of inactivity.

Activity is determined by how often events are sent with the Tracker – so you will need to send events to keep the current session active. Sessions are updated when new events are tracked. There are two timeouts that are checked: foreground and background timeout. Depending on whether the app is in foreground or backgroud, the relevant timeout is used to compare the time difference since previous event and, in case it surpasses the timeout, a new session is started.

In [Initialisation](02-initialisation.md), we discussed how to create a tracker with optional session tracking enabled and configurable foreground and background timeouts. If you enabled session tracking when creating the tracker, you can access a `ClientSession` instance using the tracker:

```cpp
auto client_session = Snowplow::get_default_tracker()->get_client_session();
```

To set the background/foreground state you will need to detect this and then set this on the `ClientSession` object like so:

```cpp
client_session.set_is_background(true || false);
```

When client sessions are used, the [`client_session`](http://iglucentral.com/schemas/com.snowplowanalytics.snowplow/client_session/jsonschema/1-0-2) context entity is added to all tracked event. This entity consists of the following properties:

| Attribute | Description | Required? |
|---|---|---|
| `userId` | An identifier for the user of the session. | Yes |
| `sessionId` | An identifier (UUID) for the session. | Yes |
| `sessionIndex` | The index of the current session for this user. | Yes |
| `eventIndex` | Optional index of the current event in the session. Signifies the order of events in which they were tracked. | No |
| `previousSessionId` | The previous session identifier (UUID) for this user. | No |
| `storageMechanism` | The mechanism that the session information has been stored on the device. | Yes |
| `firstEventId` | The optional identifier (UUID) of the first event id for this session. | No |
| `firstEventTimestamp` | Optional date-time timestamp of when the first event in the session was tracked. | No |

## Session store

The session store is used to persist the currently active session.

The tracker provides the `SqliteStorage` class that can be used as the session store. However, you may also provide a custom session store implementation. To do so, define a class that inherits from the `SessionStore` struct:

```cpp
struct SessionStore {
  virtual unique_ptr<json> get_session() = 0;
  virtual void set_session(const json &session_data) = 0;
  virtual void delete_session() = 0;
};
```

The `SessionStore` struct defines functions to retrieve, set, and delete the current session. It represents sessions in their JSON format. The three operations have the following behavior:

| Function | Description |
|---|---|
| `get_session` | Return a unique pointer to the current session or nullptr if it doesn't exist. |
| `set_session` | Persist the current session. |
| `delete_session` | Remove and reset the current session. |
