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

#include "session_configuration.hpp"
#include "../storage/sqlite_storage.hpp"

using namespace snowplow;
using std::make_shared;

SessionConfiguration::SessionConfiguration(shared_ptr<SessionStore> session_store, unsigned long long foreground_timeout, unsigned long long background_timeout) {
  m_session_store = std::move(session_store);
  m_db_name = "";
  m_foreground_timeout = foreground_timeout;
  m_background_timeout = background_timeout;

  if (!m_session_store) {
    throw std::invalid_argument("Invalid session store");
  }
}

SessionConfiguration::SessionConfiguration(const string &db_name, unsigned long long foreground_timeout, unsigned long long background_timeout) {
  m_session_store = nullptr;
  m_db_name = db_name;
  m_foreground_timeout = foreground_timeout;
  m_background_timeout = background_timeout;

  if (m_db_name == "") {
    throw std::invalid_argument("Empty database path");
  }
}

shared_ptr<SessionStore> SessionConfiguration::get_session_store() const {
  if (m_session_store) {
    return m_session_store;
  }

  return make_shared<SqliteStorage>(m_db_name);
}
