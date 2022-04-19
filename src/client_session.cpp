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

#include "client_session.hpp"
#include "constants.hpp"
#include "utils.hpp"

using namespace snowplow;
using std::lock_guard;
using std::unique_lock;
using std::shared_ptr;

ClientSession::ClientSession(shared_ptr<SessionStore> storage, unsigned long long foreground_timeout, unsigned long long background_timeout) {
  this->m_session_store = std::move(storage);
  this->m_foreground_timeout = foreground_timeout;
  this->m_background_timeout = background_timeout;

  this->m_session_storage = "SQLITE";
  this->m_is_background = false;
  this->m_is_new_session = true;

  // Check for existing session
  list<json> *session_rows = new list<json>;
  m_session_store->get_session(session_rows);

  if (session_rows->size() == 1) {
    try {
      json session_context = session_rows->front();

      this->m_user_id = session_context[SNOWPLOW_SESSION_USER_ID].get<std::string>();
      this->m_current_session_id = session_context[SNOWPLOW_SESSION_ID].get<std::string>();
      this->m_session_index = session_context[SNOWPLOW_SESSION_INDEX].get<unsigned long long>();
    } catch (...) {
      this->m_user_id = Utils::get_uuid4();
      this->m_current_session_id = "";
      this->m_session_index = 0;
      m_session_store->delete_session();
    }
  } else {
    this->m_user_id = Utils::get_uuid4();
    this->m_current_session_id = "";
    this->m_session_index = 0;
  }
  this->update_last_session_check_at();

  session_rows->clear();
  delete (session_rows);
}

// --- Public

void ClientSession::start_new_session() {
  this->m_is_new_session = true;
}

SelfDescribingJson ClientSession::update_and_get_session_context(const string &event_id) {
  json session_context_data;
  bool save_to_storage = false;

  {
    lock_guard<mutex> guard(this->m_safe_get);

    if (this->should_update_session()) {
      this->update_session(event_id);
      save_to_storage = true;
    }
    this->update_last_session_check_at();
    session_context_data = this->m_session_context_data;
  }

  if (save_to_storage) {
    m_session_store->set_session(session_context_data);
  }
  SelfDescribingJson sdj(SNOWPLOW_SCHEMA_CLIENT_SESSION, session_context_data);
  return sdj;
}

void ClientSession::set_is_background(bool is_background) {
  lock_guard<mutex> guard(this->m_safe_get);

  if (this->should_update_session()) {
    this->start_new_session();
  }
  this->update_last_session_check_at();

  this->m_is_background = is_background;
}

bool ClientSession::get_is_background() {
  lock_guard<mutex> guard(this->m_safe_get);

  return this->m_is_background;
}

// --- Private

bool ClientSession::should_update_session() {
  if (m_is_new_session) {
    return true;
  }
  unsigned long long now = Utils::get_unix_epoch_ms();
  return now < this->m_last_session_check_at || now - this->m_last_session_check_at > this->get_timeout();
}

void ClientSession::update_last_session_check_at() {
  this->m_last_session_check_at = Utils::get_unix_epoch_ms();
}

void ClientSession::update_session(const string &event_id) {
  this->m_is_new_session = false;
  this->m_first_event_id = event_id;
  this->m_previous_session_id = this->m_current_session_id;
  this->m_current_session_id = Utils::get_uuid4();
  this->m_session_index += 1;

  // update session context data
  json j;

  j[SNOWPLOW_SESSION_USER_ID] = this->m_user_id;
  j[SNOWPLOW_SESSION_ID] = this->m_current_session_id;
  j[SNOWPLOW_SESSION_INDEX] = this->m_session_index;
  j[SNOWPLOW_SESSION_STORAGE] = this->m_session_storage;

  if (this->m_previous_session_id == "") {
    j[SNOWPLOW_SESSION_PREVIOUS_ID] = nullptr;
  } else {
    j[SNOWPLOW_SESSION_PREVIOUS_ID] = this->m_previous_session_id;
  }

  if (this->m_first_event_id != "") {
    j[SNOWPLOW_SESSION_FIRST_ID] = this->m_first_event_id;
  }

  this->m_session_context_data = j;
}

unsigned long long ClientSession::get_timeout() {
  return this->m_is_background ? this->m_background_timeout : this->m_foreground_timeout;
}
