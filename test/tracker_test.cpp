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

#include "../include/base64.hpp"
#include "../include/json.hpp"
#include "../src/emitter.hpp"
#include "../src/tracker.hpp"
#include "../src/events/structured_event.hpp"
#include "../src/events/screen_view_event.hpp"
#include "../src/events/self_describing_event.hpp"
#include "../src/events/timing_event.hpp"
#include "http/test_http_client.hpp"
#include "catch.hpp"

using namespace snowplow;
using std::invalid_argument;
using std::runtime_error;
using std::to_string;

TEST_CASE("tracker") {

  // --- Emitter Mock

  class MockEmitter : public Emitter {
  private:
    bool m_started = false;
    vector<Payload> m_payloads;

  public:
    MockEmitter() : Emitter("com.acme", Emitter::Method::POST, Emitter::Protocol::HTTP, 0, 0, 0, "test-tracker.db", unique_ptr<HttpClient>(new TestHttpClient())) {}
    void start() { m_started = true; }
    void stop() { m_started = false; }
    void add(Payload payload) { m_payloads.push_back(payload); }
    void flush() { m_payloads.clear(); }
    vector<Payload> get_added_payloads() { return m_payloads; }
    bool is_started() { return m_started; }
  };

  SECTION("Mock emitter stores payloads") {
    MockEmitter e;

    e.start();
    REQUIRE(e.is_started() == true);

    Payload example;
    e.add(example);
    REQUIRE(e.get_added_payloads().size() == 1);
  }

  // --- Tracker Controls

  SECTION("Tracker singleton controls provide expected behaviour") {
    bool runtime_exception_on_not_init = false;
    try {
      Tracker *t = Tracker::instance();
    } catch (runtime_error) {
      runtime_exception_on_not_init = true;
    }
    REQUIRE(runtime_exception_on_not_init == true);

    MockEmitter e;
    Tracker *t = Tracker::init(e, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    runtime_exception_on_not_init = false;
    try {
      Tracker *t = Tracker::instance();
    } catch (runtime_error) {
      runtime_exception_on_not_init = true;
    }
    REQUIRE(runtime_exception_on_not_init == false);

    Tracker::close();
  }

  SECTION("Tracker controls should provide expected behaviour") {
    MockEmitter e;
    ClientSession cs("test-tracker.db", 5000, 5000);
    string platform = "pc";
    string app_id = "snowplow-test-suite";
    string name_space = "snowplow-testing";
    bool base64 = false;
    bool desktop_context = false;

    Tracker *t = Tracker::init(e, NULL, &cs, &platform, &app_id, &name_space, &base64, &desktop_context);

    StructuredEvent sv("hello", "world");
    t->track(sv);

    vector<Payload> payloads = e.get_added_payloads();
    REQUIRE(payloads.size() == 1);

    auto payload = payloads[0].get();
    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    REQUIRE(payload[SNOWPLOW_PLATFORM] == "pc");
    REQUIRE(payload[SNOWPLOW_APP_ID] == "snowplow-test-suite");
    REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "snowplow-testing");
    REQUIRE(payload[SNOWPLOW_EVENT] == SNOWPLOW_EVENT_STRUCTURED);
    REQUIRE(payload[SNOWPLOW_SE_ACTION] == "world");
    REQUIRE(payload[SNOWPLOW_SE_CATEGORY] == "hello");

    Subject s;
    s.set_screen_resolution(1920, 1080);
    Tracker::instance()->set_subject(&s);
    t->track(sv);

    payloads = e.get_added_payloads();
    REQUIRE(payloads.size() == 2);

    payload = payloads[1].get();
    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    REQUIRE(payload[SNOWPLOW_PLATFORM] == "pc");
    REQUIRE(payload[SNOWPLOW_APP_ID] == "snowplow-test-suite");
    REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "snowplow-testing");
    REQUIRE(payload[SNOWPLOW_EVENT] == SNOWPLOW_EVENT_STRUCTURED);
    REQUIRE(payload[SNOWPLOW_SE_ACTION] == "world");
    REQUIRE(payload[SNOWPLOW_SE_CATEGORY] == "hello");
    REQUIRE(payload[SNOWPLOW_RESOLUTION] == "1920x1080");

    s.set_screen_resolution(1080, 1920);
    Tracker::instance()->set_subject(&s);
    t->track(sv);

    payloads = e.get_added_payloads();
    REQUIRE(payloads.size() == 3);

    payload = payloads[2].get();
    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    REQUIRE(payload[SNOWPLOW_PLATFORM] == "pc");
    REQUIRE(payload[SNOWPLOW_APP_ID] == "snowplow-test-suite");
    REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "snowplow-testing");
    REQUIRE(payload[SNOWPLOW_EVENT] == SNOWPLOW_EVENT_STRUCTURED);
    REQUIRE(payload[SNOWPLOW_SE_ACTION] == "world");
    REQUIRE(payload[SNOWPLOW_SE_CATEGORY] == "hello");
    REQUIRE(payload[SNOWPLOW_RESOLUTION] == "1080x1920");

    Tracker::instance()->flush();
    REQUIRE(e.get_added_payloads().size() == 0);

    Tracker::close();
  }

  // --- Tracker Defaults

  SECTION("Tracker adds default fields to each payload") {
    MockEmitter e;
    Tracker *t = Tracker::init(e, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    REQUIRE(e.is_started() == true);

    t->track(StructuredEvent("c", "a"));

    vector<Payload> payloads = e.get_added_payloads();
    REQUIRE(payloads.size() == 1);

    auto payload = payloads[0].get();
    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    REQUIRE(payload[SNOWPLOW_PLATFORM] == "srv");
    REQUIRE(payload[SNOWPLOW_APP_ID] == "");
    REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "");

    Tracker::close();
  }

  SECTION("Tracker can change default fields") {
    MockEmitter e;

    string plat = "mob";
    string app_id = "app-id";
    string name_space = "namespace";
    bool base64 = false;
    bool desktop_context = true;

    Tracker *t = Tracker::init(e, NULL, NULL, &plat, &app_id, &name_space, &base64, &desktop_context);

    REQUIRE(e.is_started() == true);

    t->track(StructuredEvent("c", "a"));
    vector<Payload> payloads = e.get_added_payloads();

    REQUIRE(payloads.size() == 1);

    auto payload = payloads[0].get();

    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    REQUIRE(payload[SNOWPLOW_PLATFORM] == "mob");
    REQUIRE(payload[SNOWPLOW_APP_ID] == "app-id");
    REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "namespace");

    Tracker::close();
  }

  // --- Event Builders

  SECTION("StructuredEvents have appropriate defaults") {
    StructuredEvent s("category", "action");
    REQUIRE(s.category == "category");
    REQUIRE(s.action == "action");
    REQUIRE(s.get_context().size() == 0);
    REQUIRE(s.label == NULL);
    REQUIRE(s.get_true_timestamp() == NULL);
    REQUIRE(s.value == NULL);
  }

  SECTION("SelfDescribingEvents have appropriate defaults") {
    SelfDescribingJson e = SelfDescribingJson("abc", "{\"hello\": \"world\"}"_json);
    SelfDescribingEvent sde(e);
    REQUIRE(sde.event.to_string() == e.to_string());
    REQUIRE(sde.get_context().size() == 0);
    REQUIRE(sde.get_true_timestamp() == NULL);
  }

  SECTION("ScreenViewEvents have appropriate defaults") {
    ScreenViewEvent sve;
    REQUIRE(sve.get_context().size() == 0);
    REQUIRE(sve.id == NULL);
    REQUIRE(sve.name == NULL);
    REQUIRE(sve.get_true_timestamp() == NULL);
  }

  SECTION("TimingEvents have appropriate defaults") {
    TimingEvent t("cat", "variable", 123);
    REQUIRE(t.category == "cat");
    REQUIRE(t.variable == "variable");
    REQUIRE(t.get_true_timestamp() == NULL);
    REQUIRE(t.label == NULL);
    REQUIRE(t.timing == 123);
    REQUIRE(t.get_context().size() == 0);
  }

  // --- Event Tracker Functions

  SECTION("track StructuredEvent generates sane event") {
    bool is_arg_exception_empty_category;
    bool is_arg_exception_empty_action;

    MockEmitter e;
    Tracker *t = Tracker::init(e, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    StructuredEvent sv("", "hello");

    try {
      t->track(sv);
    } catch (invalid_argument) {
      is_arg_exception_empty_category = true;
    }

    sv.action = "";
    sv.category = "hello";

    try {
      t->track(sv);
    } catch (invalid_argument) {
      is_arg_exception_empty_action = true;
    }

    REQUIRE(is_arg_exception_empty_action);
    REQUIRE(is_arg_exception_empty_category);
    REQUIRE(e.get_added_payloads().size() == 0);

    sv.action = "action";
    sv.category = "category";

    t->track(sv);

    REQUIRE(e.get_added_payloads().size() == 1);

    auto payload = e.get_added_payloads()[0].get();

    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    REQUIRE(payload[SNOWPLOW_PLATFORM] == "srv");
    REQUIRE(payload[SNOWPLOW_APP_ID] == "");
    REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "");
    REQUIRE(payload[SNOWPLOW_EVENT] == SNOWPLOW_EVENT_STRUCTURED);
    REQUIRE(payload[SNOWPLOW_SE_CATEGORY] == "category");
    REQUIRE(payload[SNOWPLOW_SE_ACTION] == "action");
    REQUIRE(payload[SNOWPLOW_TIMESTAMP].size() > 10);
    REQUIRE(payload[SNOWPLOW_EID].size() > 5);
    REQUIRE(payload.find(SNOWPLOW_TRUE_TIMESTAMP) == payload.end());

    
    vector<SelfDescribingJson> context;
    context.push_back(SelfDescribingJson("hello", "{\"hello\":\"world\"}"_json));
    sv.set_context(context);
    string label = "label";
    sv.label = &label;
    string property = "property";
    sv.property = &property;
    double value = 11.11;
    sv.value = &value;
    unsigned long long ts = Utils::get_unix_epoch_ms();
    sv.set_true_timestamp(&ts);

    t->track(sv);
    auto new_payload = e.get_added_payloads()[1].get();

    REQUIRE(new_payload[SNOWPLOW_TIMESTAMP].size() > 10);
    REQUIRE(new_payload[SNOWPLOW_EID].size() > 5);
    REQUIRE(new_payload[SNOWPLOW_TIMESTAMP].size() > 10);
    unsigned long long timestamp = std::stoull(new_payload[SNOWPLOW_TIMESTAMP]);
    REQUIRE(timestamp > (ts - 1000));
    REQUIRE(timestamp < (ts + 1000));
    REQUIRE(new_payload[SNOWPLOW_SE_LABEL] == "label");
    REQUIRE(new_payload[SNOWPLOW_SE_PROPERTY] == "property");
    REQUIRE(new_payload[SNOWPLOW_SE_VALUE] == to_string(11.11));
    REQUIRE(new_payload[SNOWPLOW_TRUE_TIMESTAMP] == to_string(ts));

    Tracker::close();
  }

  SECTION("track ScreenViewEvent generates sane event") {
    MockEmitter e;
    Tracker *t = Tracker::init(e, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    ScreenViewEvent se;
    string id = "123";
    se.id = &id;
    t->track(se);

    REQUIRE(e.get_added_payloads().size() == 1);
    auto payload = e.get_added_payloads()[0].get();

    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    REQUIRE(payload[SNOWPLOW_PLATFORM] == "srv");
    REQUIRE(payload[SNOWPLOW_APP_ID] == "");
    REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "");
    REQUIRE(payload[SNOWPLOW_EID].size() > 5);
    REQUIRE(payload[SNOWPLOW_TIMESTAMP].size() > 10);
    REQUIRE(payload.count(SNOWPLOW_TRUE_TIMESTAMP) == 0);

    nlohmann::json expected;
    expected[SNOWPLOW_SV_ID] = "123";
    SelfDescribingJson sdj(SNOWPLOW_SCHEMA_SCREEN_VIEW, expected);
    SelfDescribingJson uej(SNOWPLOW_SCHEMA_UNSTRUCT_EVENT, sdj.get());

    string json = uej.to_string();
    const unsigned char *c_json = (const unsigned char *)json.c_str();

    REQUIRE(payload[SNOWPLOW_UNSTRUCTURED_ENCODED] == base64_encode(c_json, json.length()));

    se.id = NULL;
    string name = "name";
    se.name = &name;
    unsigned long long ttm = Utils::get_unix_epoch_ms();
    se.set_true_timestamp(&ttm);

    t->track(se);
    auto new_payload = e.get_added_payloads()[1].get();

    REQUIRE(new_payload[SNOWPLOW_TRUE_TIMESTAMP] == to_string(ttm));

    nlohmann::json new_expected;
    new_expected[SNOWPLOW_SV_NAME] = "name";
    SelfDescribingJson sdj_1(SNOWPLOW_SCHEMA_SCREEN_VIEW, new_expected);
    SelfDescribingJson uej_1(SNOWPLOW_SCHEMA_UNSTRUCT_EVENT, sdj_1.get());

    string json_1 = uej_1.to_string();
    const unsigned char *c_json_1 = (const unsigned char *)json_1.c_str();

    REQUIRE(new_payload[SNOWPLOW_UNSTRUCTURED_ENCODED] == base64_encode(c_json_1, json_1.length()));

    se.id = NULL;
    se.name = NULL;
    bool arg_exception_on_no_id_or_name = false;
    try {
      t->track(se);
    } catch (invalid_argument) {
      arg_exception_on_no_id_or_name = true;
    }

    REQUIRE(arg_exception_on_no_id_or_name == true);

    Tracker::close();
  }

  SECTION("track TimingEvent generates a sane event") {
    MockEmitter e;
    Tracker *t = Tracker::init(e, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    TimingEvent te("category", "variable", 123);
    t->track(te);

    REQUIRE(e.get_added_payloads().size() == 1);

    auto payload = e.get_added_payloads()[0].get();

    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    REQUIRE(payload[SNOWPLOW_PLATFORM] == "srv");
    REQUIRE(payload[SNOWPLOW_APP_ID] == "");
    REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "");
    REQUIRE(payload[SNOWPLOW_EID].size() > 5);
    REQUIRE(payload[SNOWPLOW_TIMESTAMP].size() > 10);
    REQUIRE(payload.count(SNOWPLOW_TRUE_TIMESTAMP) == 0);

    json expected;
    expected[SNOWPLOW_UT_TIMING] = 123;
    expected[SNOWPLOW_UT_CATEGORY] = "category";
    expected[SNOWPLOW_UT_VARIABLE] = "variable";
    SelfDescribingJson sdj(SNOWPLOW_SCHEMA_USER_TIMINGS, expected);
    SelfDescribingJson uej(SNOWPLOW_SCHEMA_UNSTRUCT_EVENT, sdj.get());

    string json = uej.to_string();
    const unsigned char *c_json = (const unsigned char *)json.c_str();

    REQUIRE(payload[SNOWPLOW_UNSTRUCTURED_ENCODED] == base64_encode(c_json, json.length()));

    string label = "hello world";
    te.label = &label;
    unsigned long long ts = Utils::get_unix_epoch_ms();
    te.set_true_timestamp(&ts);

    t->track(te);

    expected[SNOWPLOW_UT_LABEL] = "hello world";
    auto new_payload = e.get_added_payloads()[1].get();

    REQUIRE(new_payload[SNOWPLOW_TRUE_TIMESTAMP] == to_string(ts));

    SelfDescribingJson sde_w_label(SNOWPLOW_SCHEMA_USER_TIMINGS, expected);
    SelfDescribingJson uej_w_label(SNOWPLOW_SCHEMA_UNSTRUCT_EVENT, sde_w_label.get());
    string json_w_label = uej_w_label.to_string();

    REQUIRE(base64_decode(new_payload[SNOWPLOW_UNSTRUCTURED_ENCODED]) == json_w_label);

    TimingEvent te1("", "", 123);
    bool arg_exception_on_no_category = false;
    try {
      t->track(te1);
    } catch (invalid_argument) {
      arg_exception_on_no_category = true;
    }

    REQUIRE(arg_exception_on_no_category == true);

    TimingEvent te2("category", "", 123);
    bool arg_exception_on_no_variable = false;
    try {
      t->track(te2);
    } catch (invalid_argument) {
      arg_exception_on_no_variable = true;
    }

    REQUIRE(arg_exception_on_no_variable == true);

    Tracker::close();
  }

  SECTION("track SelfDescribingEvent generates a sane event") {
    MockEmitter e;

    bool desktop_context = false;
    Tracker *t = Tracker::init(e, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    SelfDescribingJson sdj("schema", "{ \"hello\":\"world\" }"_json);
    SelfDescribingEvent sde(sdj);
    t->track(sde);

    REQUIRE(e.get_added_payloads().size() == 1);

    auto payload = e.get_added_payloads()[0].get();

    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    REQUIRE(payload[SNOWPLOW_PLATFORM] == "srv");
    REQUIRE(payload[SNOWPLOW_APP_ID] == "");
    REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "");
    REQUIRE(payload[SNOWPLOW_EVENT] == SNOWPLOW_EVENT_SELF_DESCRIBING);
    REQUIRE(payload[SNOWPLOW_TIMESTAMP].size() > 10);
    REQUIRE(payload[SNOWPLOW_EID].size() > 5);
    REQUIRE(payload.count(SNOWPLOW_TRUE_TIMESTAMP) == 0);

    SelfDescribingJson uej(SNOWPLOW_SCHEMA_UNSTRUCT_EVENT, sdj.get());

    string json = uej.to_string();
    const unsigned char *str = (const unsigned char *)json.c_str();

    REQUIRE(payload[SNOWPLOW_UNSTRUCTURED_ENCODED] == base64_encode(str, json.length()));

    Tracker::close();
  }
}
