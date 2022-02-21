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

#ifndef STORAGE_H
#define STORAGE_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include "utils.hpp"
#include "payload.hpp"
#include "../include/sqlite3.h"
#include "../include/json.hpp"

using std::mutex;
using std::string;
using std::list;
using json = nlohmann::json;

class Storage {
private:
  static Storage *m_instance;
  static mutex m_db_get;

  Storage(const string & db_name);
  ~Storage();
  string m_db_name;
  mutex m_db_access;
  sqlite3 *m_db;
  sqlite3_stmt *m_add_stmt;

public:
  static Storage *init(const string & db_name);
  static Storage *instance();
  static void close();

  struct EventRow {
    int id;
    Payload event;
  };

  void insert_payload(Payload payload);
  void insert_update_session(json session_data);
  void select_all_event_rows(list<EventRow>* event_list);
  void select_event_row_range(list<EventRow>* event_list, int range);
  void select_all_session_rows(list<json>* session_list);
  void delete_all_event_rows();
  void delete_event_row_ids(list<int>* id_list);
  void delete_all_session_rows();
  string get_db_name();
};

#endif
