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

#include "../../include/snowplow/emitter/retry_delay.hpp"
#include "../catch.hpp"
#include "../http/test_http_client.hpp"

using namespace snowplow;
using std::chrono::milliseconds;

TEST_CASE("RetryDelay") {
  SECTION("retry delay is increasing as expected") {
    RetryDelay retry_delay(100, 2, 10, 0);
    REQUIRE(milliseconds(0) == retry_delay.get());
    retry_delay.will_retry_emit();
    REQUIRE(milliseconds(100) == retry_delay.get());
    retry_delay.will_retry_emit();
    REQUIRE(milliseconds(200) == retry_delay.get());
    retry_delay.will_retry_emit();
    REQUIRE(milliseconds(400) == retry_delay.get());
    retry_delay.will_retry_emit();
    REQUIRE(milliseconds(800) == retry_delay.get());
  }

  SECTION("retry delay stops increasing when retry count cap is reached") {
    RetryDelay retry_delay(100, 2, 1, 0);
    retry_delay.will_retry_emit();
    REQUIRE(milliseconds(100) == retry_delay.get());
    retry_delay.will_retry_emit();
    REQUIRE(milliseconds(100) == retry_delay.get());
  }

  SECTION("retry delay adds randomness") {
    RetryDelay retry_delay(100, 2, 11, 0.1);
    for (int i = 0; i < 11; i++) {
      retry_delay.will_retry_emit();
    }
    REQUIRE(retry_delay.get() != retry_delay.get());
    int expected = 102400;
    REQUIRE(milliseconds(expected - expected / 10).count() < retry_delay.get().count());
    REQUIRE(milliseconds(expected + expected / 10).count() > retry_delay.get().count());
  }

  SECTION("retry delay resets if won't retry") {
    RetryDelay retry_delay(100, 2, 10, 0);
    auto delay0 = retry_delay.get();
    retry_delay.will_retry_emit();
    auto delay1 = retry_delay.get();
    retry_delay.wont_retry_emit();
    auto delay2 = retry_delay.get();
    REQUIRE(delay1 > delay2);
    REQUIRE(delay0 == delay2);
  }
}
