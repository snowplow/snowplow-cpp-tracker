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
#include "../utils.hpp"

TEST_CASE("utils") {
  SECTION("int_list_to_string will successfully convert a list of integers to a string") {
    list<int>* int_list = new list<int>;
    int_list->push_back(1);
    int_list->push_back(2);
    int_list->push_back(3);
    int_list->push_back(4);
    int_list->push_back(5);

    REQUIRE("1,2,3,4,5" == Utils::int_list_to_string(int_list, ","));

    int_list->clear();
    delete(int_list);
  }

  SECTION("serialize_payload will successfully convert a Payload into a JSON string") {
    Payload p;
    p.add("e", "pv");
    p.add("p", "srv");
    p.add("tv", "cpp-0.1.0");

    REQUIRE("{\"e\":\"pv\",\"p\":\"srv\",\"tv\":\"cpp-0.1.0\"}" == Utils::serialize_payload(p));
  }

  SECTION("deserialize_json_str will successfully convert a JSON string into a Payload") {
    string j_str = "{\"e\":\"pv\",\"p\":\"srv\",\"tv\":\"cpp-0.1.0\"}";
    Payload p = Utils::deserialize_json_str(j_str);

    REQUIRE(p.get()["e"] == "pv");
    REQUIRE(p.get()["p"] == "srv");
    REQUIRE(p.get()["tv"] == "cpp-0.1.0");
  }
}
