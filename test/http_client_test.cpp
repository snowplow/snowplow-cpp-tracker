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

#include "../src/http_client.hpp"
#include "catch.hpp"

using namespace snowplow;

#if defined(SNOWPLOW_TEST_SUITE)

#define HTTP_TEST_URL_GET "http://com.acme.collector/i"
#define HTTP_TEST_URL_POST "http://com.acme.collector/com.snowplowanalytics.snowplow/tp2"

TEST_CASE("http_client") {
  SECTION("GET request to valid endpoint must return 200 code") {
    HttpClient::reset();

    CrackedUrl c(HTTP_TEST_URL_GET);
    string query_string = "e=pv&tv=cpp-0.1.0";
    list<int> id_list{1};

    HttpRequestResult r = HttpClient::http_get(c, query_string, id_list, false);

    REQUIRE(r.get_http_response_code() == 200);
    REQUIRE(r.is_success() == true);
    REQUIRE(r.get_row_ids().size() == id_list.size());

    list<HttpClient::Request> requests_list = HttpClient::get_requests_list();
    REQUIRE(1 == requests_list.size());

    HttpClient::Request req = requests_list.front();
    REQUIRE(HttpClient::RequestMethod::GET == req.method);
    REQUIRE(query_string == req.query_string);
    REQUIRE("" == req.post_data);
    REQUIRE(false == req.oversize);
  }

  SECTION("POST request to valid endpoint must return 200 code") {
    HttpClient::reset();

    CrackedUrl c(HTTP_TEST_URL_POST);
    string json_string = "{\"schema\":\"iglu:com.snowplowanalytics.snowplow/payload_data/jsonschema/1-0-3\",\"data\":[{\"dtm\":\"1234567890123\"}]}";
    list<int> id_list{1};

    HttpRequestResult r = HttpClient::http_post(c, json_string, id_list, false);

    REQUIRE(r.get_http_response_code() == 200);
    REQUIRE(r.is_success() == true);
    REQUIRE(r.get_row_ids().size() == id_list.size());

    list<HttpClient::Request> requests_list = HttpClient::get_requests_list();
    REQUIRE(1 == requests_list.size());

    HttpClient::Request req = requests_list.front();
    REQUIRE(HttpClient::RequestMethod::POST == req.method);
    REQUIRE("" == req.query_string);
    REQUIRE(json_string == req.post_data);
    REQUIRE(false == req.oversize);
  }
}

#endif
