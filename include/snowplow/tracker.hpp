/*
Copyright (c) 2023 Snowplow Analytics Ltd. All rights reserved.

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
#include <map>
#include "emitter/emitter.hpp"
#include "subject.hpp"
#include "client_session.hpp"
#include "events/event.hpp"
#include "configuration/tracker_configuration.hpp"

namespace snowplow {

using std::string;
using std::map;
using std::shared_ptr;

/**
 * @brief Instance of the Snowplow tracker that provides an interface to track Snowplow events.
 * 
 * You may construct and manage a Tracker instance directly or use the `Snowplow` interface for easy construction and management of Tracker instances.
 */
class Tracker {
public:
  /**
   * @brief Construct a new Tracker.
   * 
   * @param tracker_config Tracker configuration object with tracker settings.
   * @param emitter The emitter to which events are sent (required).
   * @param subject The user and device being tracked (optional).
   * @param client_session Optional client session object responsible for tracking user sessions (optional). Attach a ClientSession context to each event.
   */
  Tracker(const TrackerConfiguration &tracker_config, shared_ptr<Emitter> emitter, shared_ptr<Subject> subject = nullptr, shared_ptr<ClientSession> client_session = nullptr);

  /**
   * @brief Construct a new Tracker.
   *
   * @param emitter The emitter to which events are sent (required).
   * @param subject The user and device being tracked (optional).
   * @param client_session Optional client session object responsible for tracking user sessions (optional). Attach a ClientSession context to each event.
   * @param platform The platform the Tracker is running on, can be one of: web, mob, pc, app, srv, tv, cnsl, iot (defaults to srv).
   * @param app_id Application ID (defaults to empty string).
   * @param name_space The name of the tracker instance attached to every event (defaults to empty string).
   * @param use_base64 Whether to enable base 64 encoding (defaults to true).
   * @param desktop_context Whether to add a desktop_context, which gathers information about the device the tracker is running on, to each event (defaults to true).
   */
  Tracker(shared_ptr<Emitter> emitter, shared_ptr<Subject> subject = nullptr, shared_ptr<ClientSession> client_session = nullptr, const string &platform = SNOWPLOW_DEFAULT_PLATFORM,
          const string &app_id = SNOWPLOW_DEFAULT_APP_ID, const string &name_space = SNOWPLOW_DEFAULT_NAMESPACE, bool use_base64 = true, bool desktop_context = true);

  ~Tracker();

  /**
   * @brief Start sending events by the Emitter if not already started. Triggered automatically when tracker is initialized.
   */
  void start();

  /**
   * @brief Stop sending events by the Emitter.
   */
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
  void set_subject(shared_ptr<Subject> subject);

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

  /**
   * @brief Get the tracker namespace
   * 
   * @return string Tracker namespace
   */
  string get_namespace() const { return m_namespace; }

  /**
   * @brief Get the optional client session object
   * 
   * @return shared_ptr<ClientSession> Shared pointer to the client session or nullptr if not initialized
   */
  shared_ptr<ClientSession> get_client_session() const { return m_client_session; }

private:
  shared_ptr<Emitter> m_emitter;
  shared_ptr<Subject> m_subject;
  shared_ptr<ClientSession> m_client_session;
  string m_namespace;
  string m_app_id;
  string m_platform;
  bool m_use_base64;
  bool m_desktop_context;
};
} // namespace snowplow

#endif
