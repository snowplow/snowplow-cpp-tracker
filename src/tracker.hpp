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
#include "emitter/emitter.hpp"
#include "subject.hpp"
#include "client_session.hpp"
#include "events/event.hpp"

using std::string;
using std::vector;

namespace snowplow {
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
   * @brief Track a Snowplow event (e.g., an instance of `SelfDescribingEvent`, or `ScreenViewEvent`).
   * 
   * A Payload object will be created from the event.
   * This is passed to the configured Emitter.
   * The payload's event ID string (a UUID) is returned.
   * 
   * @param event The event to track
   * @return Tracked event ID
   */
  string track(const Event &event);

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
}

#endif
