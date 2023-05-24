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

#include "emitter_configuration.hpp"
#include "../storage/sqlite_storage.hpp"
#include "../constants.hpp"

using namespace snowplow;

EmitterConfiguration::EmitterConfiguration(shared_ptr<EventStore> event_store) : m_event_store(std::move(event_store)), m_db_name("") {
  if (!m_event_store) {
    throw std::invalid_argument("Invalid emitter event store");
  }
  shared_init();
}

EmitterConfiguration::EmitterConfiguration(const string &db_name) : m_event_store(nullptr), m_db_name(db_name) {
  if (m_db_name == "") {
    throw std::invalid_argument("Empty database path");
  }
  shared_init();
}

void EmitterConfiguration::shared_init() {
  m_batch_size = SNOWPLOW_EMITTER_DEFAULT_BATCH_SIZE;
  m_byte_limit_get = SNOWPLOW_EMITTER_DEFAULT_BYTE_LIMIT_GET;
  m_byte_limit_post = SNOWPLOW_EMITTER_DEFAULT_BYTE_LIMIT_POST;
}

void EmitterConfiguration::set_event_store(shared_ptr<EventStore> event_store) {
  if (!event_store) {
    throw std::invalid_argument("Invalid emitter event store");
  }
  m_event_store = event_store;
  m_db_name = ""; // reset `db_name` to make sure the new event store is used
}

void EmitterConfiguration::set_request_callback(const EmitterCallback &callback, EmitStatus emit_status) {
  m_callback = callback;
  m_callback_emit_status = emit_status;
}

void EmitterConfiguration::set_custom_retry_for_status_code(int http_status_code, bool retry) {
  if (http_status_code < 300) {
    throw std::invalid_argument("Retry rules can only be set for status codes >= 300");
  }

  m_custom_retry_for_status_codes.insert({http_status_code, retry});
}

shared_ptr<EventStore> EmitterConfiguration::get_event_store() const {
  if (m_event_store) {
    return m_event_store;
  }

  return make_shared<SqliteStorage>(m_db_name);
}
