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
#include "../src/emitter.hpp"

TEST_CASE("emitter") {
  SECTION("Emitter rejects urls (starting with http:// or https://)") {
    bool inv_arg_http = false;
    bool inv_arg_https = false;
    bool inv_arg_http_case = false;
    bool inv_arg_https_case = false;

    try {
      Emitter emitter("http://com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 51000, "test-emitter.db");
    }
    catch (invalid_argument) {
      inv_arg_http = true;
    }

    try {
      Emitter emitter("https://com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 51000, "test-emitter.db");
    }
    catch (invalid_argument) {
      inv_arg_https = true;
    }

    try {
      Emitter emitter("HTTP://com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 51000, "test-emitter.db");
    }
    catch (invalid_argument) {
      inv_arg_http_case = true;
    }

    try {
      Emitter emitter("HTTPS://com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 51000, "test-emitter.db");
    }
    catch (invalid_argument) {
      inv_arg_https_case = true;
    }

    REQUIRE(inv_arg_http == true);
    REQUIRE(inv_arg_https == true);
    REQUIRE(inv_arg_http_case == true);
    REQUIRE(inv_arg_https_case == true);
  }

  SECTION("Emitter setup confirmation") {
    Emitter emitter("com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 51000, "test-emitter.db");

    REQUIRE(false == emitter.is_running());
    REQUIRE("http://com.acme.collector/com.snowplowanalytics.snowplow/tp2" == emitter.get_cracked_url().to_string());
    REQUIRE(Emitter::Method::POST == emitter.get_method());
    REQUIRE(500 == emitter.get_send_limit());
    REQUIRE(52000 == emitter.get_byte_limit_post());
    REQUIRE(51000 == emitter.get_byte_limit_get());

    emitter.start();

    // Emitter can only start once
    emitter.start();
    emitter.start();

    REQUIRE(true == emitter.is_running());
    emitter.flush();
    REQUIRE(false == emitter.is_running());
    emitter.start();
    REQUIRE(true == emitter.is_running());
    emitter.stop();
    REQUIRE(false == emitter.is_running());
    emitter.flush();
    REQUIRE(false == emitter.is_running());

    Emitter emitter_1("com.acme.collector", Emitter::Method::GET, Emitter::Protocol::HTTPS, 500, 52000, 51000, "test-emitter.db");

    REQUIRE(false == emitter_1.is_running());
    REQUIRE("https://com.acme.collector/i" == emitter_1.get_cracked_url().to_string());
    REQUIRE(Emitter::Method::GET == emitter_1.get_method());
    REQUIRE(500 == emitter_1.get_send_limit());
    REQUIRE(52000 == emitter_1.get_byte_limit_post());
    REQUIRE(51000 == emitter_1.get_byte_limit_get());

    bool inv_argument_empty_uri = false;
    try {
      Emitter emitter_2("", Emitter::Method::GET, Emitter::Protocol::HTTPS, 500, 52000, 51000, "test-emitter.db"); 
    } catch (invalid_argument) {
      inv_argument_empty_uri = true;
    }
    REQUIRE(inv_argument_empty_uri == true);

    bool inv_argument_bad_url = false;
    try {
      Emitter emitter_3("../:random../gibber", Emitter::Method::GET, Emitter::Protocol::HTTPS, 500, 52000, 51000, "test-emitter.db"); 
    } catch (invalid_argument) {
      inv_argument_bad_url = true;
    }
    REQUIRE(inv_argument_bad_url == true);
  }

#if defined(SNOWPLOW_TEST_SUITE)

  SECTION("Emitter should track and remove only successful events from the database for GET requests") {
    Emitter e("com.acme.collector", Emitter::Method::GET, Emitter::Protocol::HTTPS, 500, 52000, 52000, "test-emitter.db");
    e.start();

    Payload p;
    p.add("e", "pv");

    for (int i = 0; i < 10; i++) {
      e.add(p);
    }
    e.flush();

    list<HttpClient::Request> requests = HttpClient::get_requests_list();
    REQUIRE(0 != requests.size());

    list<Storage::EventRow>* event_list = new list<Storage::EventRow>;
    Storage::instance()->select_all_event_rows(event_list);
    REQUIRE(0 == event_list->size());
    event_list->clear();

    HttpClient::set_http_response_code(404);
    e.start();

    for (int i = 0; i < 10; i++) {
      e.add(p);
    }

    event_list = new list<Storage::EventRow>;
    Storage::instance()->select_all_event_rows(event_list);
    REQUIRE(10 == event_list->size());
    event_list->clear();

    e.stop();
    HttpClient::reset();
    delete(event_list);
  }

  SECTION("Emitter should track and remove only successful events from the database for POST requests") {
    Emitter e("com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 500, 500, "test-emitter.db");
    e.start();

    Payload p;
    p.add("e", "pv");

    for (int i = 0; i < 10; i++) {
      e.add(p);
    }
    e.flush();

    list<HttpClient::Request> requests = HttpClient::get_requests_list();
    REQUIRE(0 != requests.size());

    list<Storage::EventRow>* event_list = new list<Storage::EventRow>;
    Storage::instance()->select_all_event_rows(event_list);
    REQUIRE(0 == event_list->size());
    event_list->clear();

    // Test POST 404 response
    HttpClient::set_http_response_code(404);
    e.start();

    for (int i = 0; i < 10; i++) {
      e.add(p);
    }

    event_list = new list<Storage::EventRow>;
    Storage::instance()->select_all_event_rows(event_list);
    REQUIRE(10 == event_list->size());
    event_list->clear();

    e.stop();
    HttpClient::reset();

    // Test POST combination logic
    for (int i = 0; i < 1000; i++) {
      e.add(p);
    }
    e.start();
    e.flush();

    event_list = new list<Storage::EventRow>;
    Storage::instance()->select_all_event_rows(event_list);
    REQUIRE(0 == event_list->size());
    event_list->clear();

    // Test POST single event too large logic
    p.add("tv", "pvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpv");
    e.add(p);

    HttpClient::set_http_response_code(404);
    e.start();
    e.flush();

    event_list = new list<Storage::EventRow>;
    Storage::instance()->select_all_event_rows(event_list);
    REQUIRE(0 == event_list->size());
    event_list->clear();

    HttpClient::reset();
    delete(event_list);
  }

#endif

}
