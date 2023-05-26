/*
Copyright (c) 2023 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#include "../../include/snowplow/payload/event_payload.hpp"
#include "../../include/snowplow/detail/utils/utils.hpp"
#include "../catch.hpp"
#include <map>
#include <string>

using namespace snowplow;

TEST_CASE("event payload") {
  EventPayload pl;

  SECTION("is initialized with event ID and timestamp") {
    REQUIRE(pl.get().size() == 2);
    unsigned long long time_now = Utils::get_unix_epoch_ms();
    REQUIRE(pl.get_timestamp() > (time_now - 1000));
    REQUIRE(pl.get_timestamp() < (time_now + 1000));
    REQUIRE(pl.get()["eid"] == pl.get_event_id());
    REQUIRE(pl.get()["dtm"] == std::to_string(pl.get_timestamp()));
  }
}
