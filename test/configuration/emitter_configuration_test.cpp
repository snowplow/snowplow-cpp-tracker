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

#include "../../src/configuration/emitter_configuration.hpp"
#include "../../src/storage/sqlite_storage.hpp"
#include "../catch.hpp"

using namespace snowplow;

using std::invalid_argument;

TEST_CASE("emitter configuration") {
  SECTION("accepts custom retry codes") {
    auto storage = std::make_shared<SqliteStorage>("test-emitter.db");
    EmitterConfiguration emitter_config(storage);

    emitter_config.set_custom_retry_for_status_code(505, false);
    emitter_config.set_custom_retry_for_status_code(403, true);
    auto retry_codes = emitter_config.get_custom_retry_for_status_codes();
    REQUIRE(retry_codes.size() == 2);
    REQUIRE(retry_codes.at(505) == false);
    REQUIRE(retry_codes.at(403) == true);
  }

  SECTION("doesn't accept successful custom retry codes") {
    auto storage = std::make_shared<SqliteStorage>("test-emitter.db");
    EmitterConfiguration emitter_config(storage);

    bool raised_error = false;
    try {
      emitter_config.set_custom_retry_for_status_code(202, true);
    } catch (invalid_argument) {
      raised_error = true;
    }
    REQUIRE(raised_error);
    REQUIRE(emitter_config.get_custom_retry_for_status_codes().size() == 0);
  }
}
