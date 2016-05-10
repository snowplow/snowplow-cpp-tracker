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

#ifndef STORAGE_H
#define STORAGE_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <list>
#include <thread>
#include "vendored/sqlite3.h"
#include "utils.hpp"
#include "payload.hpp"

using namespace std;

class Storage {
private:
  string m_db_name;
  sqlite3 *m_db;
  sqlite3_stmt *m_add_stmt;
  mutex m_db_access;

public:
  struct EventRow {
    int id;
    Payload event;
  };

  Storage(const string & db_name);
  ~Storage();
  void insert_payload(Payload payload);
  void select_all_event_rows(list<EventRow>* event_list);
  void select_event_row_range(list<EventRow>* event_list, int range);
  void delete_all_event_rows();
  void delete_event_row_ids(list<int>* id_list);
  string get_db_name();
};

#endif
