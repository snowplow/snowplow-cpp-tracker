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

#ifndef SQLITE_STORAGE_H
#define SQLITE_STORAGE_H

#include "event_store.hpp"
#include "session_store.hpp"
#include <string>
#include <list>
#include <mutex>
#include "../../include/json.hpp"
#include "../../include/sqlite3.h"

using std::mutex;
using std::string;
using std::list;
using json = nlohmann::json;

namespace snowplow {
/**
 * @brief Tracker SQLite storage for events and session information.
 *
 */
class SqliteStorage : public EventStore, public SessionStore {
public:
  SqliteStorage(const string &db_name);
  ~SqliteStorage();

  void add_event(const Payload &payload);
  void get_all_event_rows(list<EventRow> *event_list);
  void get_event_rows_batch(list<EventRow> *event_list, int number_to_get);
  void delete_all_event_rows();
  void delete_event_rows_with_ids(const list<int> &id_list);

  void set_session(const json &session_data);
  void get_session(list<json> *session_list);
  void delete_session();

  string get_db_name();

private:
  string m_db_name;
  mutex m_db_access;
  sqlite3 *m_db;
  sqlite3_stmt *m_add_stmt;
};
} // namespace snowplow

#endif
