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
#include "../src/subject.hpp"

TEST_CASE("subject") {
  Subject sub;
  REQUIRE(sub.get_map().size() == 0);

  SECTION("set_user_id adds a value for key uid") {
    sub.set_user_id("some-uid");
    REQUIRE(sub.get_map()["uid"] == "some-uid");
  }

  SECTION("set_screen_resolution adds a value for key res") {
    sub.set_screen_resolution(1920, 1080);
    REQUIRE(sub.get_map()["res"] == "1920x1080");
  }

  SECTION("set_viewport adds a value for key vp") {
    sub.set_viewport(1080, 1080);
    REQUIRE(sub.get_map()["vp"] == "1080x1080");
  }

  SECTION("set_color_depth adds a value for key cd") {
    sub.set_color_depth(32);
    REQUIRE(sub.get_map()["cd"] == "32");
  }

  SECTION("set_timezone adds a value for key tz") {
    sub.set_timezone("GMT");
    REQUIRE(sub.get_map()["tz"] == "GMT");
  }

  SECTION("set_language adds a value for key lang") {
    sub.set_language("EN");
    REQUIRE(sub.get_map()["lang"] == "EN");
  }
}
