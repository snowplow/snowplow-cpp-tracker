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
#include "../http_client.hpp"

//#define HTTP_TEST_URL "http://requestb.in/1gqq0of1" // url to use for get/post integration tests, comment out to skip

TEST_CASE("http_client") {

/**
  SECTION("Invalid url triggers exception for GET request") {
    // TBC - until I can think of something better to return (a message, or defined error codes)
    bool arg_exception_occurred = false;
    try {
      HttpClient::http_get("not a valid url", list<int>(), false);
    }
    catch (invalid_argument x) {
      arg_exception_occurred = true;
    }
    REQUIRE(arg_exception_occurred == true);
  }

  SECTION("Invalid url triggers exception for POST request") {
    bool arg_exception_occurred = false;
    try {
      HttpClient::http_post("not a valid url", "", list<int>(), false);
    }
    catch (invalid_argument x) {
      arg_exception_occurred = true;
    }
    REQUIRE(arg_exception_occurred == true);
  }
*/

#ifdef HTTP_TEST_URL

  SECTION("get requests work") {
    HttpRequestResult r = HttpClient::http_get("http://requestb.in/1gqq0of1", list<int>(), false);
    REQUIRE(r.get_http_response_code() == 200);
    REQUIRE(r.is_success() == true);
  }

  SECTION("https get requests work") {
    HttpRequestResult r = HttpClient::http_get("https://google.com", list<int>(), false);
    REQUIRE(r.get_http_response_code() == 200);
    REQUIRE(r.is_success() == true);
  }

  SECTION("post requests work") {
    HttpRequestResult r = HttpClient::http_post("http://requestb.in/1gqq0of1", "{\"hello\":\"world\"}", list<int>(), false);
    REQUIRE(r.get_http_response_code() == 200);
    REQUIRE(r.is_success() == true);
  }

  SECTION("GET request to valid endpoint must return 200 code") {
    HttpRequestResult r = HttpClient::http_get("http://a3b57da8.ngrok.io/i?e=pv", list<int>(), false);
    REQUIRE(r.get_http_response_code() == 200);
    REQUIRE(r.is_success() == true);
  }

  SECTION("POST request to valid endpoint must return 200 code") {
    HttpRequestResult r = HttpClient::http_post("http://a3b57da8.ngrok.io/com.snowplowanalytics.snowplow/tp2", "{\"schema\":\"iglu:com.snowplowanalytics.snowplow/payload_data/jsonschema/1-0-3\",\"data\":[{\"dtm\":\"1234567890123\"}]}", list<int>(), false);
    REQUIRE(r.get_http_response_code() == 200);
    REQUIRE(r.is_success() == true);
  }

#endif
  
}
