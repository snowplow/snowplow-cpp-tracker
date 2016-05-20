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

#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include <string>
#include <thread>
#include <chrono>
#include "self_describing_json.hpp"
#include "utils.hpp"
#include "constants.hpp"
#include "storage.hpp"
#include "../include/json.hpp"

using namespace std;
using json = nlohmann::json;

class ClientSession {
public:
  ClientSession(const string & db_name, unsigned long long foreground_timeout, unsigned long long background_timeout, unsigned long long check_interval);
  ~ClientSession();

  void start();
  void stop();
  void set_is_background(bool is_background);
  bool get_is_background();
  SelfDescribingJson get_session_context(const string & event_id);

private:
  // Constructor
  unsigned long long m_foreground_timeout;
  unsigned long long m_background_timeout;
  unsigned long long m_check_interval;

  // Context
  string m_user_id;
  string m_current_session_id;
  string m_previous_session_id;
  unsigned long long m_session_index;
  string m_session_storage;
  string m_first_event_id;

  // Updateable
  unsigned long long m_accessed_last;
  bool m_is_background;
  json m_session_context_data;

  // Daemon
  thread m_daemon_thread;
  mutex m_run_check;
  mutex m_safe_get;
  bool m_is_running;
  void run();
  void update_session();
  void update_accessed_last();
  void update_session_context_data();
  bool is_time_in_range(unsigned long long start, unsigned long long check, unsigned long long range);
  bool is_running();
};

#endif
