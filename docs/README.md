# C++ Tracker

The [Snowplow C++ Tracker](https://github.com/snowplow/snowplow-cpp-tracker) allows you to track Snowplow events from your C++ apps, games and servers.

There are three basic types of object you will create when using the Snowplow C++ Tracker: subjects, emitters, and trackers.

* A subject represents a user whose events are tracked.
* A tracker constructs events and sends them to an emitter.
* The emitter then sends the event to the endpoint you configure; a valid Snowplow Collector.
