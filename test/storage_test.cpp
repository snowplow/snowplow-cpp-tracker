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

#include "catch.hpp"
#include "../src/storage.hpp"

TEST_CASE("storage") {
  Storage *storage = Storage::init("test1.db");
  REQUIRE("test1.db" == storage->get_db_name());
  storage->delete_all_event_rows();
  storage->delete_all_session_rows();

  SECTION("singleton controls should work as expected") {
    Storage::close();

    bool runtime_error_not_init = false;
    try {
        Storage::instance();
    } catch (runtime_error) {
        runtime_error_not_init = true;
    }
    REQUIRE(runtime_error_not_init == true);

    Storage::init("test1.db");

    runtime_error_not_init = false;
    try {
        Storage::instance();
    } catch (runtime_error) {
        runtime_error_not_init = true;
    }
    REQUIRE(runtime_error_not_init == false);
  }

  SECTION("database should throw exceptions for unmanageable errors") {
    Storage::close();

    bool runtime_error_bad_db_name = false;
    try {
        Storage::init("~/");
    } catch (runtime_error) {
        runtime_error_bad_db_name = true;
    }
    REQUIRE(runtime_error_bad_db_name == true);

    Storage::init("test1.db");
  }

  SECTION("should be able to insert,select and delete Payload objects to and from the database") {
    Payload p;
    p.add("e", "pv");
    p.add("p", "srv");
    p.add("tv", "cpp-0.1.0");

    // INSERT 50 rows
    for (int i = 0; i < 50; i++) {
      storage->insert_payload(p);
    }

    // SELECT one row
    list<Storage::EventRow>* event_list = new list<Storage::EventRow>;
    storage->select_all_event_rows(event_list);
    REQUIRE(50 == event_list->size());

    for (list<Storage::EventRow>::iterator it = event_list->begin(); it != event_list->end(); ++it) {
      REQUIRE("pv" == it->event.get()["e"]);
      REQUIRE("srv" == it->event.get()["p"]);
      REQUIRE("cpp-0.1.0" == it->event.get()["tv"]);
    }
    event_list->clear();

    storage->select_event_row_range(event_list, 100);
    REQUIRE(50 == event_list->size());
    event_list->clear();
    storage->select_event_row_range(event_list, 5);
    REQUIRE(5 == event_list->size());
    
    // DELETE rows by id
    list<int>* id_list = new list<int>;
    for (list<Storage::EventRow>::iterator it = event_list->begin(); it != event_list->end(); ++it) {
      id_list->push_back(it->id);
    }
    storage->delete_event_row_ids(id_list);
    event_list->clear();
    id_list->clear();
    delete(id_list);

    storage->select_event_row_range(event_list, 100);
    REQUIRE(45 == event_list->size());
    event_list->clear();

    // DELETE all rows
    storage->delete_all_event_rows();
    storage->select_all_event_rows(event_list);
    REQUIRE(0 == event_list->size());
    event_list->clear();

    // Delete memory for list
    delete(event_list);
    storage->delete_all_event_rows();
    Storage::close();
  }

  SECTION("should be able to insert only one session object into the database") {
    list<json>* session_rows = new list<json>;

    // Insert and check row
    json j = "{\"storage\":\"SQLITE\",\"previousSessionId\":null}"_json;
    storage->insert_update_session(j);
    storage->select_all_session_rows(session_rows);

    REQUIRE(1 == session_rows->size());
    REQUIRE("{\"previousSessionId\":null,\"storage\":\"SQLITE\"}" == session_rows->front().dump());
    session_rows->clear();

    // Check we can only insert one row
    for (int i = 0; i < 50; i++) {
      storage->insert_update_session(j);
    }
    storage->select_all_session_rows(session_rows);

    REQUIRE(1 == session_rows->size());
    REQUIRE("{\"previousSessionId\":null,\"storage\":\"SQLITE\"}" == session_rows->front().dump());
    session_rows->clear();

    // Check we can update the row values
    j = "{\"storage\":\"SQLITE\",\"previousSessionId\":\"a_value\"}"_json;
    storage->insert_update_session(j);
    storage->select_all_session_rows(session_rows);

    REQUIRE(1 == session_rows->size());
    REQUIRE("{\"previousSessionId\":\"a_value\",\"storage\":\"SQLITE\"}" == session_rows->front().dump());
    session_rows->clear();

    // Delete memory for list
    delete(session_rows);
    storage->delete_all_session_rows();
    Storage::close();
  }
}
