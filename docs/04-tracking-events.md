# Tracking specific events

Snowplow has been built to enable you to track a wide range of events that occur when users interact with your apps.

We provide several built-in event classes to help you track different kinds of events. When instantiated, their objects can be passed to the `tracker.track()` methods to send events to the Snowplow collector. The event classes range from single purpose ones, such as `ScreenViewEvent`, to the more complex but flexible `SelfDescribingEvent`, which can be used to track any kind of user behaviour. We strongly recommend using `SelfDescribingEvent` for your tracking, as it allows you to design custom event types to match your business requirements. [This post](https://snowplowanalytics.com/blog/2020/01/24/re-thinking-the-structure-of-event-data/) on our blog, "Re-thinking the structure of event data" might be informative here.

The tracker provides the following event classes for tracking events out of the box:

| **Function** | **Description** |
| --- | --- |
| `StructuredEvent` | Tracks a Snowplow custom structured event |
| `SelfDescribingEvent` | Tracks a Snowplow custom unstructured event |
| `ScreenViewEvent` | Tracks the user viewing a screen within the application |
| `TimingEvent` | Tracks a timing event |

Snowplow events are all processed into the same format, regardless of the event type (and regardless of the tracker language used). Read about the different properties and fields of events in the [Snowplow Tracker Protocol](https://docs.snowplowanalytics.com/docs/collecting-data/collecting-from-own-applications/snowplow-tracker-protocol/).

## Custom event context

Event context is an incredibly powerful aspect of Snowplow tracking, which allows you to create very rich data. It is based on the same self-describing JSON schemas as the self-describing events. Using event context, you can add any details you like to your events, as long as you can describe them in a self-describing JSON schema.

Each schema will describe a single "entity". All of an event's entities together form the event context. There is no limit to how many entities can be attached to one event.

Note that context can be added to any event type, not just self-describing events. This means that even a simple event type like a screen view can hold complex and extensive information – reducing the chances of data loss and the amount of modelling (JOINs etc.) needed in modelling, while increasing the value of each event, and the sophistication of the possible use cases.

The entities you provide are validated against their schemas as the event is processed (during the enrich phase). If there is a mistake or mismatch, the event is processed as a Bad Event.

Once defined, an entity can be attached to any kind of event. This is also an important point; it means your tracking is as DRY as possible. Using the same "user" or "image" or "search result" (etc.) entities throughout your tracking reduces error, and again makes the data easier to model.

Each tracking type provides a `set_context()` method which accepts a vector of context entities:

```cpp
vector<SelfDescribingJson> context;
```

If a visitor arrives on a page advertising a movie, the dictionary for a single custom context entity in the list might look like this:

```cpp
vector<SelfDescribingJson> context;
SelfDescribingJson sdj(
  "iglu:com.acme_company/movie_poster/jsonschema/2-1-1",
  "{\"movie_name\":\"Solaris\",\"poster_country\":\"JP\"}"_json
);
context.push_back(sdj);
```

This is how to fire a structured event with the above custom context entity:

```cpp
StructuredEvent se("category", "action");
se.set_context(context);

Tracker::instance()->track(se);
```

Note that even though there is only one custom context entity attached to the event, it still needs to be placed in a list.

**INFO**: We use the excellent json library from Github community member [Niels (nlohmann)](https://github.com/nlohmann) for all JSON parsing. For more information on using this library please visit the [Technical information on Github](https://github.com/nlohmann/json).

## Optional true-timestamp argument

Each event class supports an optional true-timestamp argument; this allows you to provide the true-timestamp attached to this event to help with the timing of events in multiple timezones. The timestamp should be in milliseconds since the Unix epoch.

Here is an example tracking a structured event and supplying the optional true-timestamp argument:

```cpp
StructuredEvent se("category", "action");

// As it is optional you will need to pass the address for this value
unsigned long long true_tstamp = "1368725287000";
se.set_true_timestamp(&true_tstamp);

Tracker::instance()->track(se);
```

## Track SelfDescribing/Unstructured events with "SelfDescribingEvent"

Use the `SelfDescribingEvent` type to track a custom event which consists of a name and an unstructured set of properties. This is useful when:

* You want to track event types which are proprietary/specific to your business (i.e. not already part of Snowplow), or
* You want to track events which have unpredictable or frequently changing properties

A self-describing JSON has two keys, `schema` and `data`. The `schema` value should point to a valid self-describing JSON schema. They are called self-describing because the schema will specify the fields allowed in the data value. Read more about how schemas are used with Snowplow [here](https://docs.snowplowanalytics.com/docs/understanding-tracking-design/understanding-schemas-and-validation/).

After events have been collected by the event collector, they are validated to ensure that the properties match the self-describing JSONs. Mistakes (e.g. extra fields, or incorrect types) will result in events being processed as Bad Events. This means that only high-quality, valid events arrive in your data storage or real-time stream.

Your schemas must be accessible to your pipeline to allow this validation. We provide [Iglu](https://docs.snowplowanalytics.com/docs/pipeline-components-and-applications/iglu/) for schema management. If you are a paid Snowplow customer, you can manage your schemas through your console. Check out our [Ruby tracker Rails](https://github.com/snowplow-incubator/snowplow-ruby-tracker-examples) example to see how we included schemas in the Snowplow Micro testing pipeline in that app.

`SelfDescribingEvent` provides the following properties:

| **Argument** | **Description** | **Required?** | **Validation** |
| --- | --- | --- | --- |
| `event` | The properties of the event | Yes | SelfDescribingJson |

Example:

```cpp
// Create a JSON containing your data
json data = "{\"level\":5,\"saveId\":\"ju302\",\"hardMode\":true}"_json;

// Create a new SelfDescribingJson
SelfDescribingJson sdj("iglu:com.example_company/save-game/jsonschema/1-0-2", data);

SelfDescribingEvent sde(sdj);
Tracker::instance()->track(sde);
```

For more on JSON schema, refer to [this page](https://docs.snowplowanalytics.com/docs/understanding-tracking-design/understanding-schemas-and-validation/).

## Track screen views with "ScreenViewEvent"

Use the `ScreenViewEvent` type to track a user viewing a screen (or equivalent) within your app. This is the page view equivalent for apps that are not webpages.

| **Argument** | **Description** | **Required?** | **Type** |
| --- | --- | --- | --- |
| `name` | Human-readable name for this screen | No  | *string |
| `id` | Unique identifier for this screen | No  | *string |

Although name and id are not individually required, at least one must be provided or the event will fail validation and subsequently throw an exception.

The event uses the following Iglu schema: [iglu:com.snowplowanalytics.snowplow/screen_view/jsonschema/1-0-0](http://iglucentral.com/schemas/com.snowplowanalytics.snowplow/screen_view/jsonschema/1-0-0).

Example:

```cpp
string name = "Screen ID - 5asd56";

ScreenViewEvent sve;
sve.name = &name;

Tracker::instance()->track(sve);
```

## Track structured events with "StructuredEvent"

Use the `StructuredEvent` type to track a custom event happening in your app which fits the Google Analytics-style structure of having up to five fields (with only the first two required).  This method provides a halfway-house between tracking fully user-defined self-describing events and out-of-the box predefined events. This event type can be used to track many types of user activity, as it is somewhat customizable.

As these fields are fairly arbitrary, we recommend following the advice in this table how to define structured events. It's important to be consistent throughout the business about how each field is used.

| **Argument** | **Description** | **Required?** | **Validation** |
| --- | --- | --- | --- |
| `category` | The grouping of structured events which this `action` belongs to | Yes | string |
| `action` | Defines the type of user interaction which this event involves | Yes | string |
| `label` | A string to provide additional dimensions to the event data | No  | *string |
| `property` | A string describing the object or the action performed on it | No  | *string |
| `value` | A value to provide numerical data about the event | No  | *double |

Example:

```cpp
StructuredEvent se("shop", "add-to-basket"); // constructor takes category and action
se.property = "pcs";
se.value = 25.6;

Tracker::instance()->track(se);
```

## Track timing events with "TimingEvent"

Use the `TimingEvent` type to track user timing events such as how long resources take to load.

Its properties are as follows:

| **Argument** | **Description** | **Required?** | **Validation** |
| --- | --- | --- | --- |
| `category` | The category of the event | Yes | *string |
| `variable` | The variable of the event | Yes | *string |
| `timing` | The timing of the event | Yes | *int64 |
| `label` | The label of the event | No  | *string |

The event uses the following Iglu schema: [iglu:com.snowplowanalytics.snowplow/timing/jsonschema/1-0-0](http://iglucentral.com/schemas/com.snowplowanalytics.snowplow/timing/jsonschema/1-0-0).

Example:

```cpp
TimingEvent te("category", "variable", 123);
Tracker::instance()->track(te);
```
