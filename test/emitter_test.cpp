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

#include "../src/emitter.hpp"
#include "../src/payload/event_payload.hpp"
#include "http/test_http_client.hpp"
#include "../src/storage/sqlite_storage.hpp"
#include "catch.hpp"
#include "http/test_http_client.hpp"

using namespace snowplow;
using std::invalid_argument;
using std::make_tuple;
using std::tuple;
using std::unique_ptr;
using std::vector;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;

string track_sample_event(Emitter &emitter) {
  emitter.start();
  EventPayload payload;
  payload.add("e", "pv");
  emitter.add(payload);
  emitter.flush();
  return payload.get_event_id();
}

TEST_CASE("emitter") {
  auto storage = std::make_shared<SqliteStorage>("test-emitter.db");

  SECTION("Emitter rejects urls (starting with http:// or https://)") {
    bool inv_arg_http = false;
    bool inv_arg_https = false;
    bool inv_arg_http_case = false;
    bool inv_arg_https_case = false;

    try {
      Emitter emitter("http://com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 51000, storage, unique_ptr<HttpClient>(new TestHttpClient()));
    } catch (invalid_argument) {
      inv_arg_http = true;
    }

    try {
      Emitter emitter("https://com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 51000, storage, unique_ptr<HttpClient>(new TestHttpClient()));
    } catch (invalid_argument) {
      inv_arg_https = true;
    }

    try {
      Emitter emitter("HTTP://com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 51000, storage, unique_ptr<HttpClient>(new TestHttpClient()));
    } catch (invalid_argument) {
      inv_arg_http_case = true;
    }

    try {
      Emitter emitter("HTTPS://com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 51000, storage, unique_ptr<HttpClient>(new TestHttpClient()));
    } catch (invalid_argument) {
      inv_arg_https_case = true;
    }

    REQUIRE(inv_arg_http == true);
    REQUIRE(inv_arg_https == true);
    REQUIRE(inv_arg_http_case == true);
    REQUIRE(inv_arg_https_case == true);
  }

  SECTION("Emitter setup confirmation") {
    Emitter emitter("com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 51000, storage, unique_ptr<HttpClient>(new TestHttpClient()));

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

    Emitter emitter_1("com.acme.collector", Emitter::Method::GET, Emitter::Protocol::HTTPS, 500, 52000, 51000, storage, unique_ptr<HttpClient>(new TestHttpClient()));

    REQUIRE(false == emitter_1.is_running());
    REQUIRE("https://com.acme.collector/i" == emitter_1.get_cracked_url().to_string());
    REQUIRE(Emitter::Method::GET == emitter_1.get_method());
    REQUIRE(500 == emitter_1.get_send_limit());
    REQUIRE(52000 == emitter_1.get_byte_limit_post());
    REQUIRE(51000 == emitter_1.get_byte_limit_get());

    bool inv_argument_empty_uri = false;
    try {
      Emitter emitter_2("", Emitter::Method::GET, Emitter::Protocol::HTTPS, 500, 52000, 51000, storage, unique_ptr<HttpClient>(new TestHttpClient()));
    } catch (invalid_argument) {
      inv_argument_empty_uri = true;
    }
    REQUIRE(inv_argument_empty_uri == true);

    bool inv_argument_bad_url = false;
    try {
      Emitter emitter_3("../:random../gibber", Emitter::Method::GET, Emitter::Protocol::HTTPS, 500, 52000, 51000, storage, unique_ptr<HttpClient>(new TestHttpClient()));
    } catch (invalid_argument) {
      inv_argument_bad_url = true;
    }
    REQUIRE(inv_argument_bad_url == true);
  }

  SECTION("Emitter should track and remove only successful events from the database for GET requests") {
    Emitter emitter("com.acme.collector", Emitter::Method::GET, Emitter::Protocol::HTTPS, 500, 52000, 52000, storage, unique_ptr<HttpClient>(new TestHttpClient()));
    emitter.start();

    Payload payload;
    payload.add("e", "pv");

    for (int i = 0; i < 10; i++) {
      emitter.add(payload);
    }
    emitter.flush();

    list<TestHttpClient::Request> requests = TestHttpClient::get_requests_list();
    REQUIRE(0 != requests.size());

    list<EventRow> *event_list = new list<EventRow>;
    storage->get_all_event_rows(event_list);
    REQUIRE(0 == event_list->size());
    event_list->clear();

    TestHttpClient::set_http_response_code(404);
    emitter.start();

    for (int i = 0; i < 10; i++) {
      emitter.add(payload);
    }

    event_list = new list<EventRow>;
    storage->get_all_event_rows(event_list);
    REQUIRE(10 == event_list->size());
    event_list->clear();

    emitter.stop();
    TestHttpClient::reset();
    delete (event_list);
  }

  SECTION("Emitter should track and remove only successful events from the database for POST requests") {
    Emitter emitter("com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 500, 500, storage, unique_ptr<HttpClient>(new TestHttpClient()));
    emitter.start();

    Payload payload;
    payload.add("e", "pv");

    for (int i = 0; i < 10; i++) {
      emitter.add(payload);
    }
    emitter.flush();

    list<TestHttpClient::Request> requests = TestHttpClient::get_requests_list();
    REQUIRE(0 != requests.size());

    list<EventRow> *event_list = new list<EventRow>;
    storage->get_all_event_rows(event_list);
    REQUIRE(0 == event_list->size());
    event_list->clear();

    // Test POST 404 response
    TestHttpClient::set_http_response_code(404);
    emitter.start();

    for (int i = 0; i < 10; i++) {
      emitter.add(payload);
    }

    event_list = new list<EventRow>;
    storage->get_all_event_rows(event_list);
    REQUIRE(10 == event_list->size());
    event_list->clear();

    emitter.stop();
    TestHttpClient::reset();

    // Test POST combination logic
    for (int i = 0; i < 1000; i++) {
      emitter.add(payload);
    }
    emitter.start();
    emitter.flush();

    event_list = new list<EventRow>;
    storage->get_all_event_rows(event_list);
    REQUIRE(0 == event_list->size());
    event_list->clear();

    // Test POST single event too large logic
    payload.add("tv", "pvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpvpv");
    emitter.add(payload);

    TestHttpClient::set_http_response_code(404);
    emitter.start();
    emitter.flush();

    event_list = new list<EventRow>;
    storage->get_all_event_rows(event_list);
    REQUIRE(0 == event_list->size());
    event_list->clear();

    TestHttpClient::reset();
    delete (event_list);
  }

  SECTION("triggers callback for all emit statuses") {
    Emitter emitter("com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 500, 500, storage, unique_ptr<HttpClient>(new TestHttpClient()));
    vector<tuple<list<string>, EmitStatus>> calls;
    emitter.set_request_callback(
        [&](list<string> event_ids, EmitStatus status) {
          calls.push_back(make_tuple(event_ids, status));
        },
        EmitStatus::SUCCESS | EmitStatus::FAILED_WILL_RETRY | EmitStatus::FAILED_WONT_RETRY);

    // calls for successful event
    TestHttpClient::set_http_response_code(200);
    string event_id = track_sample_event(emitter);
    sleep_for(milliseconds(500));
    REQUIRE(1 == calls.size());
    REQUIRE(1 == std::get<0>(calls[0]).size());
    REQUIRE(event_id == std::get<0>(calls[0]).front());
    REQUIRE(EmitStatus::SUCCESS == std::get<1>(calls[0]));
    calls.clear();
    TestHttpClient::reset();

    // calls for failed with retry event
    TestHttpClient::set_temporary_response_code(500);
    event_id = track_sample_event(emitter);
    sleep_for(milliseconds(500));
    REQUIRE(2 == calls.size());
    REQUIRE(event_id == std::get<0>(calls[0]).front());
    REQUIRE(event_id == std::get<0>(calls[1]).front());
    REQUIRE(EmitStatus::FAILED_WILL_RETRY == std::get<1>(calls[0]));
    REQUIRE(EmitStatus::SUCCESS == std::get<1>(calls[1]));
    calls.clear();
    TestHttpClient::reset();

    // calls for failed with no retry event
    TestHttpClient::set_temporary_response_code(422);
    event_id = track_sample_event(emitter);
    sleep_for(milliseconds(500));
    REQUIRE(1 == calls.size());
    REQUIRE(event_id == std::get<0>(calls[0]).front());
    REQUIRE(EmitStatus::FAILED_WONT_RETRY == std::get<1>(calls[0]));
    TestHttpClient::reset();
  }

  SECTION("doesn't trigger callbacks for not subscribed emit statuses") {
    Emitter emitter("com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 500, 500, storage, unique_ptr<HttpClient>(new TestHttpClient()));
    vector<tuple<list<string>, EmitStatus>> calls;
    emitter.set_request_callback(
        [&](list<string> event_ids, EmitStatus status) {
          calls.push_back(make_tuple(event_ids, status));
        },
        EmitStatus::FAILED_WONT_RETRY);

    // doesn't call for successful event
    TestHttpClient::set_http_response_code(200);
    track_sample_event(emitter);
    sleep_for(milliseconds(500));
    REQUIRE(0 == calls.size());
    TestHttpClient::reset();

    // doesn't call for failed with retry event
    TestHttpClient::set_temporary_response_code(500);
    track_sample_event(emitter);
    sleep_for(milliseconds(500));
    REQUIRE(0 == calls.size());
    TestHttpClient::reset();

    // calls for failed with no retry event
    TestHttpClient::set_temporary_response_code(422);
    string event_id = track_sample_event(emitter);
    sleep_for(milliseconds(500));
    REQUIRE(1 == calls.size());
    REQUIRE(event_id == std::get<0>(calls[0]).front());
    REQUIRE(EmitStatus::FAILED_WONT_RETRY == std::get<1>(calls[0]));
    TestHttpClient::reset();
  }

  SECTION("Emitter should not retry failed events for no-retry status codes") {
    Emitter emitter("com.acme.collector", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 500, 500, storage, unique_ptr<HttpClient>(new TestHttpClient()));

    TestHttpClient::set_http_response_code(200); // success, don't retry
    track_sample_event(emitter);
    REQUIRE(1 == TestHttpClient::get_requests_list().size());
    TestHttpClient::reset();

    TestHttpClient::set_temporary_response_code(501); // retry
    track_sample_event(emitter);
    REQUIRE(2 == TestHttpClient::get_requests_list().size());
    TestHttpClient::reset();

    TestHttpClient::set_temporary_response_code(422); // don't retry for this code
    track_sample_event(emitter);
    REQUIRE(1 == TestHttpClient::get_requests_list().size());
    TestHttpClient::reset();

    emitter.set_custom_retry_for_status_code(501, false);
    emitter.set_custom_retry_for_status_code(422, true);

    TestHttpClient::set_temporary_response_code(501); // don't retry
    track_sample_event(emitter);
    REQUIRE(1 == TestHttpClient::get_requests_list().size());
    TestHttpClient::reset();

    TestHttpClient::set_temporary_response_code(422); // retry
    track_sample_event(emitter);
    REQUIRE(2 == TestHttpClient::get_requests_list().size());
    TestHttpClient::reset();

    emitter.stop();
  }
}
