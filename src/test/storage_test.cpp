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

#include "../vendored/catch.hpp"
#include "../storage.hpp"

TEST_CASE("storage") {
  Storage storage("test.db");
  REQUIRE("test.db" == storage.get_db_name());
  storage.delete_all_event_rows();

  Payload p;
  p.add("e", "pv");
  p.add("p", "srv");
  p.add("tv", "cpp-0.1.0");

  SECTION("should be able to insert,select and delete Payload objects to and from the database") {
    // INSERT 50 rows
    for (int i = 0; i < 50; i++) {
      storage.insert_payload(p);
    }

    // SELECT one row
    list<Storage::EventRow>* event_list = new list<Storage::EventRow>;
    storage.select_all_event_rows(event_list);
    REQUIRE(50 == event_list->size());

    for (list<Storage::EventRow>::iterator it = event_list->begin(); it != event_list->end(); ++it) {
      REQUIRE("pv" == it->event.get()["e"]);
      REQUIRE("srv" == it->event.get()["p"]);
      REQUIRE("cpp-0.1.0" == it->event.get()["tv"]);
    }
    event_list->clear();

    storage.select_event_row_range(event_list, 100);
    REQUIRE(50 == event_list->size());
    event_list->clear();
    storage.select_event_row_range(event_list, 5);
    REQUIRE(5 == event_list->size());
    
    // DELETE rows by id
    list<int>* id_list = new list<int>;
    for (list<Storage::EventRow>::iterator it = event_list->begin(); it != event_list->end(); ++it) {
      id_list->push_back(it->id);
    }
    storage.delete_event_row_ids(id_list);
    event_list->clear();
    id_list->clear();
    delete(id_list);

    storage.select_event_row_range(event_list, 100);
    REQUIRE(45 == event_list->size());
    event_list->clear();

    // DELETE all rows
    storage.delete_all_event_rows();
    storage.select_all_event_rows(event_list);
    REQUIRE(0 == event_list->size());
    event_list->clear();

    // Delete memory for list
    delete(event_list);
  }
}
