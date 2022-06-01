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

#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include <string>
#include <mutex>
#include "payload/self_describing_json.hpp"
#include "thirdparty/json.hpp"
#include "storage/session_store.hpp"
#include "configuration/session_configuration.hpp"
#include "constants.hpp"

using std::string;
using std::mutex;
using std::shared_ptr;
using json = nlohmann::json;

namespace snowplow {
/**
 * @brief Keeps track of users sessions and can be configured to timeout after a certain amount of inactivity.
 * 
 * Activity is determined by how often events are sent with the Tracker. Sessions are updated on on each tracked event.
 * 
 * Tracker automatically appends session information as a context entity to each tracked event.
 * Schema for the context entity: iglu:com.snowplowanalytics.snowplow/client_session/jsonschema/1-0-2
 */
class ClientSession {
public:
  /**
   * @brief Construct a new Client Session object
   * 
   * @param session_config Session configuration with timeouts and session store.
   */
  ClientSession(const SessionConfiguration &session_config);

  /**
   * @brief Construct a new Client Session object
   * 
   * @param session_store Defines the database where session data will be read and stored
   * @param foreground_timeout Timeout in ms for updating the session when the app is in foreground
   * @param background_timeout Timeout in ms for updating the session when the app is in background
   */
  ClientSession(shared_ptr<SessionStore> session_store, unsigned long long foreground_timeout = SNOWPLOW_SESSION_DEFAULT_TIMEOUT, unsigned long long background_timeout = SNOWPLOW_SESSION_DEFAULT_TIMEOUT);

  /**
   * @brief Forces a new session to be started when next event is tracked.
   */
  void start_new_session();

  /**
   * @brief Set the background state.
   * 
   * In addition to setting the background state, the function checks for timeouts in the current session.
   * It checks using timeouts belonging to the state before this transition, i.e., if the app is in
   * foreground, and the `set_is_background` function is called with `true`, the timeouts are checked
   * using the `foreground_timeout`.
   * In case the session timed out, a new session is scheduled to be started and will be started
   * when next event is tracked.
   * 
   * @param is_background New background state
   */
  void set_is_background(bool is_background);

  /**
   * @brief Get the background state.
   * 
   * @return true if set to background
   * @return false if not in background
   */
  bool get_is_background();

  /**
   * @brief Returns the session context while updating the session if necessary.
   *
   * Session is updated in case the first event is tracked, or the time since last tracked event is
   * longer than timeout passed in constructor (background timeout if in background or
   * foreground timeout if in foreground). Each update is persisted in the SQLite database.
   *
   * @param event_id Tracked event ID
   * @param event_timestamp Tracked event timestamp
   * @return SelfDescribingJson JSON with the session context
   */
  SelfDescribingJson update_and_get_session_context(const string &event_id, unsigned long long event_timestamp);

  /**
   * @brief Get the background timeout setting
   * 
   * @return unsigned long long Background timeout in ms
   */
  unsigned long long get_background_timeout() const { return m_background_timeout; }
  
  /**
   * @brief Get the foreground timeout setting
   * 
   * @return unsigned long long Foreground timeout in ms
   */
  unsigned long long get_foreground_timeout() const { return m_foreground_timeout; }

private:
  // Constructor
  shared_ptr<SessionStore> m_session_store;
  unsigned long long m_foreground_timeout;
  unsigned long long m_background_timeout;

  // Context
  string m_user_id;
  string m_current_session_id;
  string m_previous_session_id;
  unsigned long long m_session_index;
  unsigned long long m_event_index;
  string m_session_storage;
  string m_first_event_id;

  // Updateable
  unsigned long long m_last_session_check_at;
  bool m_is_background;
  json m_session_context_data;
  bool m_is_new_session;

  // Session management
  mutex m_safe_get;
  void update_session(const string &event_id, unsigned long long event_timestamp);
  void update_last_session_check_at();
  bool should_update_session();
  unsigned long long get_timeout();
  string timestamp_to_string(unsigned long long timestamp);
};
}

#endif
