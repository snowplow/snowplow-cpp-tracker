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

#include "catch.hpp"
#include "../src/http_request_result.hpp"

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

  SECTION("the http_response_code and error_code should be 200 and 0 when oversized") {
    HttpRequestResult httpRequestResult(123, 404, list<int>(), true);
    REQUIRE(httpRequestResult.get_http_response_code() == 200);
    REQUIRE(httpRequestResult.is_success() == true);
  }

  SECTION("the default constructor should return nothing for getter functions") {
    HttpRequestResult httpRequestResult;
    REQUIRE(httpRequestResult.get_http_response_code() == 0);
    REQUIRE(httpRequestResult.is_success() == false);
  }
}
