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

#ifndef SESSION_CONFIGURATION_H
#define SESSION_CONFIGURATION_H

#include <string>
#include "../storage/session_store.hpp"
#include "../http/http_client.hpp"
#include "../emitter/emit_status.hpp"
#include "../constants.hpp"

namespace snowplow {

using std::shared_ptr;
using std::string;

/**
 * @brief Configuration object containing settings used to initialize client session tracking.
 * 
 * Client sessions keep track of users sessions and can be configured to timeout after a certain amount of inactivity (see `foreground_timeout` and `background_timeout`).
 * 
 * The tracker automatically appends session information as a context entity with events.
 * See the `ClienSession` class for more details.
 */
class SessionConfiguration {
public:
  /**
   * @brief Construct a new configuration for client session tracking.
   * 
   * @param session_store Defines the database where session data will be read and stored.
   * @param foreground_timeout Timeout in ms for updating the session when the app is in foreground.
   * @param background_timeout Timeout in ms for updating the session when the app is in background.
   */
  SessionConfiguration(shared_ptr<SessionStore> session_store, unsigned long long foreground_timeout = SNOWPLOW_SESSION_DEFAULT_TIMEOUT, unsigned long long background_timeout = SNOWPLOW_SESSION_DEFAULT_TIMEOUT);

  /**
   * @brief Construct a new configuration for client session tracking.
   * 
   * @param db_name Relative path to an SQLite database where session data will be read and stored.
   * @param foreground_timeout Timeout in ms for updating the session when the app is in foreground.
   * @param background_timeout Timeout in ms for updating the session when the app is in background.
   */
  SessionConfiguration(const string &db_name, unsigned long long foreground_timeout = SNOWPLOW_SESSION_DEFAULT_TIMEOUT, unsigned long long background_timeout = SNOWPLOW_SESSION_DEFAULT_TIMEOUT);

  /**
   * @brief Get the session store.
   * 
   * If SQLite database path was passed in the constructor, this method initializes a new `SqliteStorage` instance.
   * 
   * @return shared_ptr<SessionStore> The database to use for session info storage.
   */
  shared_ptr<SessionStore> get_session_store() const;

  /**
   * @brief Get the db_name path set in the constructor.
   * 
   * @return string Path to the SQLite database to be used as session store (optional).
   */
  string get_db_name() const { return m_db_name; }

  /**
   * @return unsigned long long Timeout in ms for updating the session when the app is in foreground.
   */
  unsigned long long get_foreground_timeout() const { return m_foreground_timeout; }

  /**
   * @return unsigned long long Timeout in ms for updating the session when the app is in background.
   */
  unsigned long long get_background_timeout() const { return m_background_timeout; }

  /**
   * @brief Set the session store object
   * 
   * @param session_store Defines the database to use for session storage
   */
  void set_session_store(shared_ptr<SessionStore> session_store) { m_session_store = std::move(session_store); }

private:
  unsigned long long m_foreground_timeout;
  unsigned long long m_background_timeout;
  shared_ptr<SessionStore> m_session_store;
  string m_db_name;
};
} // namespace snowplow

#endif
