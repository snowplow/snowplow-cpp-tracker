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

#include "../../src/storage/sqlite_storage.hpp"
#include "../catch.hpp"

using namespace snowplow;
using std::runtime_error;

TEST_CASE("SQLite storage") {
  SECTION("database name reflects initialization") {
    SqliteStorage storage("test1.db");
    REQUIRE("test1.db" == storage.get_db_name());
  }

  SECTION("database should throw exceptions for unmanageable errors") {
    bool runtime_error_bad_db_name = false;
    try {
      SqliteStorage("~/");
    } catch (runtime_error) {
      runtime_error_bad_db_name = true;
    }
    REQUIRE(runtime_error_bad_db_name == true);

    SqliteStorage("test1.db");
  }

  SECTION("should be able to insert,select and delete Payload objects to and from the database") {
    SqliteStorage storage("test1.db");
    Payload p;
    p.add("e", "pv");
    p.add("p", "srv");
    p.add("tv", "cpp-0.1.0");

    // INSERT 50 rows
    for (int i = 0; i < 50; i++) {
      storage.add_event(p);
    }

    // SELECT one row
    list<EventRow> *event_list = new list<EventRow>;
    storage.get_all_event_rows(event_list);
    REQUIRE(50 == event_list->size());

    for (list<EventRow>::iterator it = event_list->begin(); it != event_list->end(); ++it) {
      REQUIRE("pv" == it->event.get()["e"]);
      REQUIRE("srv" == it->event.get()["p"]);
      REQUIRE("cpp-0.1.0" == it->event.get()["tv"]);
    }
    event_list->clear();

    storage.get_event_rows_batch(event_list, 100);
    REQUIRE(50 == event_list->size());
    event_list->clear();
    storage.get_event_rows_batch(event_list, 5);
    REQUIRE(5 == event_list->size());

    // DELETE rows by id
    list<int> id_list;
    for (list<EventRow>::iterator it = event_list->begin(); it != event_list->end(); ++it) {
      id_list.push_back(it->id);
    }
    storage.delete_event_rows_with_ids(id_list);
    event_list->clear();

    storage.get_event_rows_batch(event_list, 100);
    REQUIRE(45 == event_list->size());
    event_list->clear();

    // DELETE all rows
    storage.delete_all_event_rows();
    storage.get_all_event_rows(event_list);
    REQUIRE(0 == event_list->size());
    event_list->clear();

    // Delete memory for list
    delete (event_list);
    storage.delete_all_event_rows();
  }

  SECTION("should be able to insert only one session object into the database") {
    SqliteStorage storage("test1.db");

    // Insert and check row
    json j = "{\"storage\":\"SQLITE\",\"previousSessionId\":null}"_json;
    storage.set_session(j);
    auto session = std::unique_ptr<json>(storage.get_session());

    REQUIRE(session);
    REQUIRE("{\"previousSessionId\":null,\"storage\":\"SQLITE\"}" == session->dump());

    // Check we can only insert one row
    for (int i = 0; i < 50; i++) {
      storage.set_session(j);
    }
    session = std::unique_ptr<json>(storage.get_session());

    REQUIRE(session);
    REQUIRE("{\"previousSessionId\":null,\"storage\":\"SQLITE\"}" == session->dump());

    // Check we can update the row values
    j = "{\"storage\":\"SQLITE\",\"previousSessionId\":\"a_value\"}"_json;
    storage.set_session(j);
    session = std::unique_ptr<json>(storage.get_session());

    REQUIRE(session);
    REQUIRE("{\"previousSessionId\":\"a_value\",\"storage\":\"SQLITE\"}" == session->dump());

    // Delete session
    storage.delete_session();
  }
}
