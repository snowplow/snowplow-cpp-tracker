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

#include "../../src/http/http_request_result.hpp"
#include "../catch.hpp"

using namespace snowplow;

TEST_CASE("http_request_result") {
  SECTION("is_success should be set only if the error code is zero and the response code is 200") {
    HttpRequestResult httpRequestResult(0, 200, list<int>(), false);
    REQUIRE(httpRequestResult.is_success() == true);
    httpRequestResult = HttpRequestResult(0, 500, list<int>(), false);
    REQUIRE(httpRequestResult.is_success() == false);
    httpRequestResult = HttpRequestResult(-1, 200, list<int>(), false);
    REQUIRE(httpRequestResult.is_success() == false);
  }

  SECTION("the http return code should only be set if the error code is zero") {
    HttpRequestResult httpRequestResult(123, 200, list<int>(), false);
    REQUIRE(httpRequestResult.get_http_response_code() == -1);
    httpRequestResult = HttpRequestResult(0, 999, list<int>(), false);
    REQUIRE(httpRequestResult.get_http_response_code() == 999);
  }

  SECTION("should not retry if success") {
    HttpRequestResult httpRequestResult(0, 200, list<int>(), true);
    REQUIRE(httpRequestResult.is_success() == true);
    REQUIRE(httpRequestResult.should_retry(map<int, bool>()) == false);
  }

  SECTION("should not retry if oversized") {
    HttpRequestResult httpRequestResult(0, 500, list<int>(), true);
    REQUIRE(httpRequestResult.is_success() == false);
    REQUIRE(httpRequestResult.should_retry(map<int, bool>()) == false);
  }

  SECTION("should retry for internal errors") {
    HttpRequestResult httpRequestResult(1, 200, list<int>(), false);
    REQUIRE(httpRequestResult.is_success() == false);
    REQUIRE(httpRequestResult.should_retry(map<int, bool>()) == true);
  }

  SECTION("should retry for 5xx status codes") {
    HttpRequestResult httpRequestResult(0, 501, list<int>(), false);
    REQUIRE(httpRequestResult.is_success() == false);
    REQUIRE(httpRequestResult.should_retry(map<int, bool>()) == true);
  }

  SECTION("should not retry for no-retry status codes") {
    HttpRequestResult httpRequestResult(0, 422, list<int>(), false);
    REQUIRE(httpRequestResult.get_http_response_code() == 422);
    REQUIRE(httpRequestResult.is_success() == false);
    REQUIRE(httpRequestResult.should_retry(map<int, bool>()) == false);
  }

  SECTION("should retry according to custom status code rules") {
    map<int, bool> custom_rules;
    custom_rules.insert({501, false});
    custom_rules.insert({422, true});

    HttpRequestResult httpRequestResult1(0, 501, list<int>(), false);
    REQUIRE(httpRequestResult1.should_retry(custom_rules) == false);

    HttpRequestResult httpRequestResult2(0, 422, list<int>(), false);
    REQUIRE(httpRequestResult2.should_retry(custom_rules) == true);
  }

  SECTION("the default constructor should return nothing for getter functions") {
    HttpRequestResult httpRequestResult;
    REQUIRE(httpRequestResult.get_http_response_code() == 0);
    REQUIRE(httpRequestResult.is_success() == false);
  }
}
