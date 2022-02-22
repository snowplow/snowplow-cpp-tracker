/*
Copyright (c) 2022 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#ifndef TRACKER_H
#define TRACKER_H

#include <string>
#include "emitter.hpp"
#include "subject.hpp"
#include "client_session.hpp"
#include "self_describing_json.hpp"

using std::string;
using std::vector;

/**
 * @brief Singleton object that provides an interface to track Snowplow events.
 */
class Tracker {
public:
  /**
   * @brief Initializes and returns the singleton tracker instance.
   * 
   * @param emitter The emitter to which events are sent (required).
   * @param subject The user being tracked (optional).
   * @param client_session Client session object responsible for tracking user sessions (optional). Attach a ClientSession context to each event.
   * @param platform The platform the Tracker is running on, can be one of: web, mob, pc, app, srv, tv, cnsl, iot (defaults to srv).
   * @param app_id Application ID (defaults to empty string).
   * @param name_space The name of the tracker instance attached to every event (defaults to empty string).
   * @param use_base64 Whether to enable base 64 encoding (defaults to true).
   * @param desktop_context Whether to add a desktop_context, which gathers information about the device the tracker is running on, to each event (defaults to true).
   * @return Tracker* 
   */
  static Tracker *init(Emitter &emitter, Subject *subject, ClientSession *client_session, string *platform, 
    string *app_id, string *name_space, bool *use_base64, bool *desktop_context);

  /**
   * @brief Returns the initialized singleton tracker instance.
   * 
   * @return Tracker* Tracker instance.
   */
  static Tracker *instance();

  /**
   * @brief Clean up the tracker to be used in main destructor.
   */
  static void close();

  /**
   * @brief  Event to capture custom consumer interactions without the need to define a custom schema.
   */
  class StructuredEvent {
  public:
    /**
     * @brief Name for the group of objects you want to track e.g. "media", "ecomm".
     */
    string category; // required

    /**
     * @brief Defines the type of user interaction for the web object.
     * 
     * E.g., "play-video", "add-to-basket".
     */
    string action; // required

    /**
     * @brief Identifies the specific object being actioned.
     * 
     * E.g., ID of the video being played, or the SKU or the product added-to-basket.
     */
    string *label;
  
    /**
     * @brief Describes the object or the action performed on it.
     * 
     * This might be the quantity of an item added to basket
     */
    string *property;

    /**
     * @brief Quantifies or further describes the user action.
     * 
     * This might be the price of an item added-to-basket, or the starting time of the video where play was just pressed.
     */
    double *value;

    /**
     * @brief Unix timestamp (in ms) when the event was created. Assigned automatically.
     * @deprecated Use the `true_timestamp` instead.
     */
    unsigned long long timestamp;

    /**
     * @brief ID of the event (UUID v4) that is assigned automatically.
     * @deprecated The ability to set custom event ID will be removed in the future
     */
    string event_id;

    /**
     * @brief Optional, user-defined Unix timestamp (in ms) for the event to override the automatically assigned one.
     */
    unsigned long long *true_timestamp;

    /**
     * @brief Context entities added to the event.
     * 
     */
    vector<SelfDescribingJson> contexts;

    /**
     * @brief Construct a new Structured Event object
     * 
     * @param category Name for the group of objects you want to track e.g. "media", "ecomm".
     * @param action Defines the type of user interaction for the web object.
     */
    StructuredEvent(string category, string action);
  };

  /**
   * @brief Event to track custom information that does not fit into the out-of-the box events.
   * 
   * Self-describing events are a [data structure based on JSON Schemas](https://docs.snowplowanalytics.com/docs/understanding-tracking-design/understanding-schemas-and-validation/)
   * and can have arbitrarily many fields.
   * To define your own custom self-describing event, you must create a JSON schema for that
   * event and upload it to an [Iglu Schema Repository](https://github.com/snowplow/iglu) using
   * [igluctl](https://docs.snowplowanalytics.com/docs/open-source-components-and-applications/iglu/)
   * (or if a Snowplow BDP customer, you can use the
   * [Snowplow BDP Console UI](https://docs.snowplowanalytics.com/docs/understanding-tracking-design/managing-data-structures/)
   * or [Data Structures API](https://docs.snowplowanalytics.com/docs/understanding-tracking-design/managing-data-structures-via-the-api-2/)).
   * Snowplow uses the schema to validate that the JSON containing the event properties is well-formed.
   */
  class SelfDescribingEvent {
  public:
    /**
     * @brief Main properties of the self-describing event including it's schema and body
     */
    SelfDescribingJson event; // required

    /**
     * @brief Unix timestamp (in ms) when the event was created. Assigned automatically.
     * @deprecated Use the `true_timestamp` instead.
     */
    unsigned long long timestamp;

    /**
     * @brief ID of the event (UUID v4) that is assigned automatically.
     * @deprecated The ability to set custom event ID will be removed in the future
     */
    string event_id;

    /**
     * @brief Optional, user-defined Unix timestamp (in ms) for the event to override the automatically assigned one.
     */
    unsigned long long *true_timestamp;

    /**
     * @brief Context entities added to the event.
     */
    vector<SelfDescribingJson> contexts;

    /**
     * @brief Construct a new Self Describing Event object
     * 
     * @param event Main properties of the self-describing event including it's schema and body
     */
    SelfDescribingEvent(SelfDescribingJson event);
  };

  /**
   * @brief Event to track user viewing a screen within the application.
   * 
   * Schema for the event: iglu:com.snowplowanalytics.snowplow/screen_view/jsonschema/1-0-0
   */
  class ScreenViewEvent {
  public:
    /**
     * @brief The name of the screen viewed.
     */
    string *name;

    /**
     * @brief The id of screen that was viewed.
     */
    string *id;

    /**
     * @brief Unix timestamp (in ms) when the event was created. Assigned automatically.
     * @deprecated Use the `true_timestamp` instead.
     */
    unsigned long long timestamp;

    /**
     * @brief ID of the event (UUID v4) that is assigned automatically.
     * @deprecated The ability to set custom event ID will be removed in the future
     */
    string event_id;

    /**
     * @brief Optional, user-defined Unix timestamp (in ms) for the event to override the automatically assigned one.
     */
    unsigned long long *true_timestamp;

    /**
     * @brief Context entities added to the event.
     */
    vector<SelfDescribingJson> contexts;

    /**
     * @brief Construct a new Screen View Event object
     */
    ScreenViewEvent();
  };

  /**
   * @brief Event used to track user timing events such as how long resources take to load.
   * 
   * Schema: iglu:com.snowplowanalytics.snowplow/timing/jsonschema/1-0-0
   */
  class TimingEvent {
  public:
    /**
     * @brief Defines the timing category.
     */
    string category; // required

    /**
     * @brief Defines the timing variable measured.
     */
    string variable; // required

    /**
     * @brief Represents the time.
     */
    unsigned long long timing; // required

    /**
     * @brief An optional string to further identify the timing event.
     */
    string *label;

    /**
     * @brief Unix timestamp (in ms) when the event was created. Assigned automatically.
     * @deprecated Use the `true_timestamp` instead.
     */
    unsigned long long timestamp;

    /**
     * @brief ID of the event (UUID v4) that is assigned automatically.
     * @deprecated The ability to set custom event ID will be removed in the future
     */
    string event_id;

    /**
     * @brief Optional, user-defined Unix timestamp (in ms) for the event to override the automatically assigned one.
     */
    unsigned long long *true_timestamp;

    /**
     * @brief Context entities added to the event.
     * 
     */
    vector<SelfDescribingJson> contexts;

    /**
     * @brief Construct a new Timing Event object
     * 
     * @param category Defines the timing category.
     * @param variable Defines the timing variable measured.
     * @param timing Represents the time.
     */
    TimingEvent(string category, string variable, unsigned long long timing);
  };

  void start();
  void stop();

  /**
   * @brief Used to ensure all of your events are sent before closing your application.

   * It is a blocking call that will send everything in the database and then will join the daemon thread to the calling thread.
   */
  void flush();

  /**
   * @brief Set the optional subject object after tracker initialization.
   * 
   * @param subject Instance of subject
   */
  void set_subject(Subject *subject);

  /**
   * @brief Track en event with custom payload. We do not recommend using this function. Instead, track events using the predefined functions for each event.
   * 
   * @param p Event payload
   * @param event_id ID of the event
   * @param contexts Vector of custom contexts
   */
  void track(Payload p, const string & event_id, vector<SelfDescribingJson> &contexts);

  /**
   * @brief Track a Snowplow custom structured event which fits the Google Analytics-style structure of having up to five fields.
   */
  void track_struct_event(StructuredEvent);

  /**
   * @brief Track the user viewing a screen within the application.
   */
  void track_screen_view(ScreenViewEvent);

  /**
   * @brief Track a timing event.
   */
  void track_timing(TimingEvent);

  /**
   * @brief Track a Snowplow custom unstructured event.
   */
  void track_self_describing_event(SelfDescribingEvent);

private:
  static Tracker *m_instance;
  static mutex m_tracker_get;

  Tracker(Emitter &emitter, Subject *subject, ClientSession *client_session, string *platform, 
    string *app_id, string *name_space, bool *use_base64, bool *desktop_context);
  ~Tracker();

  Emitter &m_emitter;
  Subject *m_subject;
  ClientSession *m_client_session;
  string m_namespace;
  string m_app_id;
  string m_platform;
  bool m_use_base64;
  bool m_desktop_context;
};

#endif
