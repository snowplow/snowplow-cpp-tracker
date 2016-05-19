/*
Copyright (c) 2016 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#include "client_session.hpp"

ClientSession::ClientSession(const string & db_name, unsigned long long foreground_timeout, unsigned long long background_timeout, unsigned long long check_interval) {
  this->m_db_name = db_name;
  this->m_foreground_timeout = foreground_timeout;
  this->m_background_timeout = background_timeout;
  this->m_check_interval = check_interval;

  this->m_session_storage = "SQLITE";
  this->m_is_running = false;
  this->m_is_background = false;

  // Check for existing session
  list<json>* session_rows = new list<json>;
  Storage::instance(this->m_db_name)->select_all_session_rows(session_rows);

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
      Storage::instance(this->m_db_name)->delete_all_session_rows();
    }
  } else {
    this->m_user_id = Utils::get_uuid4();
    this->m_current_session_id = "";
    this->m_session_index = 0;
  }

  session_rows->clear();
  delete(session_rows);

  this->update_session();
  this->update_accessed_last();
  this->update_session_context_data();
  Storage::instance(this->m_db_name)->insert_update_session(this->m_session_context_data);
}

ClientSession::~ClientSession() {
  this->stop();
}

// --- Public

void ClientSession::start() {
  lock_guard<mutex> guard(this->m_run_check);
  if (this->m_is_running) {
    return;
  }
  this->m_is_running = true;
  this->m_daemon_thread = thread(&ClientSession::run, this);
}

void ClientSession::stop() {
  unique_lock<mutex> locker(this->m_run_check);
  if (this->m_is_running == true) {
    this->m_is_running = false;
    locker.unlock();
    this->m_daemon_thread.join();
  } else {
    locker.unlock();
  }
}

SelfDescribingJson ClientSession::get_session_context(const string & event_id) {
  lock_guard<mutex> guard(this->m_safe_get);

  this->update_accessed_last();

  if (this->m_first_event_id == "") {
    this->m_first_event_id = event_id;
    this->m_session_context_data[SNOWPLOW_SESSION_FIRST_ID] = this->m_first_event_id;
  }

  SelfDescribingJson sdj(SNOWPLOW_SCHEMA_CLIENT_SESSION, this->m_session_context_data);
  return sdj;
}

void ClientSession::set_is_background(bool is_background) {
  this->m_is_background = is_background;
}

bool ClientSession::get_is_background() {
  return this->m_is_background;
}

// --- Private

void ClientSession::run() {
  do {
    this_thread::sleep_for(chrono::milliseconds(this->m_check_interval));

    unsigned long long check_time = Utils::get_unix_epoch_ms();
    unsigned long long range = this->m_is_background ? this->m_background_timeout : this->m_foreground_timeout;

    if (!this->is_time_in_range(this->m_accessed_last, check_time, range)) {
      this->update_session();
      this->update_accessed_last();
      this->update_session_context_data();
      Storage::instance(this->m_db_name)->insert_update_session(this->m_session_context_data);
    }
  } while (this->is_running());
}

void ClientSession::update_session() {
  this->m_previous_session_id = this->m_current_session_id;
  this->m_current_session_id = Utils::get_uuid4();
  this->m_session_index += 1;
}

void ClientSession::update_accessed_last() {
  this->m_accessed_last = Utils::get_unix_epoch_ms();
}

void ClientSession::update_session_context_data() {
  lock_guard<mutex> guard(this->m_safe_get);

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

bool ClientSession::is_time_in_range(unsigned long long start, unsigned long long check, unsigned long long range) {
  return start > (check - range);
}

bool ClientSession::is_running() {
  lock_guard<mutex> guard(this->m_run_check);
  return this->m_is_running;
}
