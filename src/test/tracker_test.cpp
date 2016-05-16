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
#include "../tracker.hpp"
#include "../emitter.hpp"
#include "../vendored/json.hpp"
#include "../vendored/base64.hpp"

TEST_CASE("tracker") {

  class MockEmitter : public Emitter {
  private:
    bool started = false;
    vector<Payload> p;

  public:
    MockEmitter() : Emitter("abc123.duck", Emitter::Method::POST, Emitter::Protocol::HTTP, 0, 0, 0, "test.db") {}
    void start() {
      started = true;
    }
    void stop() {
      started = false;
    }
    void add(Payload payload) {
      p.push_back(payload);
    }
    void flush() {}
    vector<Payload> get_added_payloads() { return p; }
    bool is_started() { return started; }
  };

  SECTION("mock emitter stores payloads") {
    MockEmitter me;

    me.start();    
    REQUIRE(me.is_started() == true);

    Payload example;   
    me.add(example);
    REQUIRE(me.get_added_payloads().size() == 1);
  }

  SECTION("Tracker sets appropriate fields to each payload") {
    MockEmitter me;
    string track = string("");
    Tracker t(track, me);
    
    REQUIRE(me.is_started() == true);

    vector<SelfDescribingJson> v;
    Payload p;
    t.track(p, v);
    vector<Payload> payloads = me.get_added_payloads();
    REQUIRE(payloads.size() == 1);

    auto payload = payloads[0].get();

    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    REQUIRE(payload[SNOWPLOW_PLATFORM] == "srv");
    REQUIRE(payload[SNOWPLOW_APP_ID] == "");
    REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "");
  }

  //SECTION("page view events have appropriate defaults") {
  //  unsigned long long time_now = Utils::get_unix_epoch_ms();
  //  Tracker::PageViewEvent p("http://hello/world");
  //  REQUIRE(p.page_url == "http://hello/world");
  //  REQUIRE(p.page_title == "");
  //  REQUIRE(p.referrer == "");
  //  REQUIRE(p.timestamp > (time_now - 1000));
  //  REQUIRE(p.timestamp < (time_now + 1000));
  //  REQUIRE(p.true_timestamp == 0);
  //  REQUIRE(p.contexts.size() == 0);
  //}

  SECTION("structured events have appropriate defaults") {
    unsigned long long time_now = Utils::get_unix_epoch_ms();
    Tracker::StructuredEvent s("category", "action");
    REQUIRE(s.category == "category");
    REQUIRE(s.action == "action");
    REQUIRE(s.contexts.size() == 0);
    REQUIRE(s.event_id.size() > 5);
    REQUIRE(s.label == NULL);
    REQUIRE(s.timestamp > (time_now - 1000));
    REQUIRE(s.timestamp < (time_now + 1000));
    REQUIRE(s.true_timestamp == NULL);
    REQUIRE(s.value == NULL);
  }

  SECTION("SelfDescribingEvents have appropriate defaults") {
    unsigned long long time_now = Utils::get_unix_epoch_ms();
    SelfDescribingJson e = SelfDescribingJson("abc", "{\"hello\": \"world\"}"_json);
    Tracker::SelfDescribingEvent sde(e);
    REQUIRE(sde.event.to_string() == e.to_string());
    REQUIRE(sde.contexts.size() == 0);
    REQUIRE(sde.event_id.size() > 5);
    REQUIRE(sde.timestamp > time_now - 1000);
    REQUIRE(sde.timestamp < time_now + 1000);
    REQUIRE(sde.true_timestamp == 0);
  }

  SECTION("ScreenViewEvents have appropriate defaults") {
    unsigned long long time_now = Utils::get_unix_epoch_ms();
    Tracker::ScreenViewEvent sve;
    REQUIRE(sve.contexts.size() == 0);
    REQUIRE(sve.event_id.size() > 5);
    REQUIRE(sve.id == NULL);
    REQUIRE(sve.name == NULL);
    REQUIRE(sve.timestamp > time_now - 1000);
    REQUIRE(sve.timestamp < time_now + 1000);
    REQUIRE(sve.true_timestamp == NULL);
  }  

  SECTION("TimingEvents have appropriate defaults") {
    unsigned long long time_now = Utils::get_unix_epoch_ms();
    Tracker::TimingEvent t("cat", "variable", 123);
    REQUIRE(t.category == "cat");
    REQUIRE(t.variable == "variable");
    REQUIRE(t.timestamp > time_now - 1000);
    REQUIRE(t.timestamp < time_now + 1000);
    REQUIRE(t.true_timestamp == NULL);
    REQUIRE(t.label == NULL);
    REQUIRE(t.timing == 123);
    REQUIRE(t.contexts.size() == 0);
    REQUIRE(t.event_id.size() > 5);
  }

  //SECTION("EcommerceTransactionItemEvent's have appropriate defaults") {
  //  unsigned long long time_now = Utils::get_unix_epoch_ms();
  //  Tracker::EcommerceTransactionItemEvent e("cat", 99.99);
  //  REQUIRE(e.sku == "cat");
  //  REQUIRE(e.price == 99.99);
  //  REQUIRE(e.category == "");
  //  REQUIRE(e.contexts.size() == 0);
  //  REQUIRE(e.event_id.size() > 5);
  //  REQUIRE(e.name == "");
  //  REQUIRE(e.quantity == 1);
  //}

  /*SECTION("EcommerceTransactionEvent's have appropriate defaults") {
    unsigned long long time_now = Utils::get_unix_epoch_ms();
    Tracker::EcommerceTransactionEvent e("order", 99.99);
    REQUIRE(e.order_id == "order");
    REQUIRE(e.total_value == 99.99);
    REQUIRE(e.affiliation == "");
    REQUIRE(e.city == "");
    REQUIRE(e.contexts.size() == 0);
    REQUIRE(e.country == "");
    REQUIRE(e.currency == "");
    REQUIRE(e.event_id.size() > 5);
    REQUIRE(e.items.size() == 0);
    REQUIRE(e.shipping == 0.0);
    REQUIRE(e.state == "");
    REQUIRE(e.tax_value == 0.0);
    REQUIRE(e.true_timestamp == 0);
    REQUIRE(e.timestamp > time_now - 1000);
    REQUIRE(e.timestamp < time_now + 1000);
  }*/

  //SECTION("Track page view event tracks a pageview event") {
  //  MockEmitter e;
  //  string url = "";
  //  Tracker t(url, e);

  //  Tracker::PageViewEvent pve("page!");
  //  t.track_page_view_event(pve);

  //  REQUIRE(e.get_added_payloads().size() == 1);
  //  auto payload = e.get_added_payloads()[0].get();

  //  REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
  //  REQUIRE(payload[SNOWPLOW_PLATFORM] == "srv");
  //  REQUIRE(payload[SNOWPLOW_APP_ID] == "");
  //  REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "");

  //  REQUIRE(payload[SNOWPLOW_EVENT] == SNOWPLOW_EVENT_PAGE_VIEW);
  //  REQUIRE(payload[SNOWPLOW_PAGE_URL] == "page!");
  //  REQUIRE(payload[SNOWPLOW_PAGE_TITLE] == "");
  //  REQUIRE(payload[SNOWPLOW_PAGE_REFR] == "");
  //  REQUIRE(payload[SNOWPLOW_TIMESTAMP].size() > 10);
  //  REQUIRE(payload[SNOWPLOW_EID].size() > 5);
  //  REQUIRE(payload.find(SNOWPLOW_TRUE_TIMESTAMP) == payload.end()); // ttm is not present!

  //  pve.true_timestamp = Utils::get_unix_epoch_ms();
  //  pve.page_title = "hello world";
  //  pve.referrer = "http://google.com";

  //  t.track_page_view_event(pve);

  //  auto new_payload = e.get_added_payloads()[1].get();
  //  REQUIRE(new_payload[SNOWPLOW_PAGE_TITLE] == "hello world");
  //  REQUIRE(new_payload[SNOWPLOW_PAGE_REFR] == "http://google.com");
  //  REQUIRE(new_payload[SNOWPLOW_TRUE_TIMESTAMP].size() > 10);

  //  // page url is mandatory
  //  pve.page_url = "";
  //  bool is_arg_exception_no_page_url = false;
  //  try {
  //    t.track_page_view_event(pve);
  //  }
  //  catch (invalid_argument) {
  //    is_arg_exception_no_page_url = true;
  //  }

  //  REQUIRE(is_arg_exception_no_page_url == true);
  //}

  SECTION("track structured event tracks a structured event") {
    bool is_arg_exception_empty_category;
    bool is_arg_exception_empty_action;

    MockEmitter e; 
    string url = "somewhere";
    Tracker t(url, e);
    
    Tracker::StructuredEvent sv("", "hello");

    try { t.track_struct_event(sv); } 
    catch (invalid_argument) { is_arg_exception_empty_category = true; }

    sv.action = "";
    sv.category = "hello";

    try { t.track_struct_event(sv); }
    catch (invalid_argument) { is_arg_exception_empty_action = true; }

    REQUIRE(is_arg_exception_empty_action);
    REQUIRE(is_arg_exception_empty_category);

    REQUIRE(e.get_added_payloads().size() == 0);

    sv.action = "action";
    sv.category = "category";

    t.track_struct_event(sv);

    REQUIRE(e.get_added_payloads().size() == 1);
    auto payload = e.get_added_payloads()[0].get();

    // default stuff for all events
    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    REQUIRE(payload[SNOWPLOW_PLATFORM] == "srv");
    REQUIRE(payload[SNOWPLOW_APP_ID] == "");
    REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "");

    // required
    REQUIRE(payload[SNOWPLOW_EVENT] == SNOWPLOW_EVENT_STRUCTURED);
    REQUIRE(payload[SNOWPLOW_SE_CATEGORY] == "category");
    REQUIRE(payload[SNOWPLOW_SE_ACTION] == "action");

    REQUIRE(payload[SNOWPLOW_TIMESTAMP].size() > 10);
    REQUIRE(payload[SNOWPLOW_EID].size() > 5);
    REQUIRE(payload.find(SNOWPLOW_TRUE_TIMESTAMP) == payload.end());

    sv.contexts = vector<SelfDescribingJson>();
    sv.contexts.push_back(SelfDescribingJson("hello", "{\"hello\":\"world\"}"_json));
    string label = "label";
    sv.label = &label;
    string property = "property";
    sv.property = &property;
    double value = 11.11;
    sv.value = &value;
    unsigned long long ts = Utils::get_unix_epoch_ms();
    sv.true_timestamp = &ts;

    t.track_struct_event(sv);
    auto new_payload = e.get_added_payloads()[1].get();

    REQUIRE(new_payload[SNOWPLOW_TIMESTAMP].size() > 10);
    REQUIRE(new_payload[SNOWPLOW_EID].size() > 5);
    REQUIRE(new_payload[SNOWPLOW_TIMESTAMP].size() > 10);
    REQUIRE(new_payload[SNOWPLOW_SE_LABEL] == "label");
    REQUIRE(new_payload[SNOWPLOW_SE_PROPERTY] == "property");
    REQUIRE(new_payload[SNOWPLOW_SE_VALUE] == to_string(11.11));
    REQUIRE(new_payload[SNOWPLOW_TRUE_TIMESTAMP] == to_string(ts));   
  }

  SECTION("track screen view events generates sane event") {
    MockEmitter e;
    string url = "somewhere";
    Tracker t(url, e);

    Tracker::ScreenViewEvent se;
    string id = "123";
    se.id = &id;
    t.track_screen_view(se);

    REQUIRE(e.get_added_payloads().size() == 1);
    auto payload = e.get_added_payloads()[0].get();

    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    REQUIRE(payload[SNOWPLOW_PLATFORM] == "srv");
    REQUIRE(payload[SNOWPLOW_APP_ID] == "");
    REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "");

    REQUIRE(payload[SNOWPLOW_SV_ID] == id);
    REQUIRE(payload.count(SNOWPLOW_SV_NAME) == 0);
    REQUIRE(payload.count(SNOWPLOW_TRUE_TIMESTAMP) == 0);

    REQUIRE(payload[SNOWPLOW_TIMESTAMP].size() > 10);
    REQUIRE(payload[SNOWPLOW_EID].size() > 5);

    se.id = NULL;
    string name = "name";
    se.name = &name;
    unsigned long long ttm = Utils::get_unix_epoch_ms();
    se.true_timestamp = &ttm;

    t.track_screen_view(se);
    auto new_payload = e.get_added_payloads()[1].get();

    REQUIRE(new_payload.count(SNOWPLOW_SV_ID) == 0);
    REQUIRE(new_payload[SNOWPLOW_SV_NAME] == "name");
    REQUIRE(new_payload[SNOWPLOW_TRUE_TIMESTAMP] ==  to_string(ttm));

    se.id = NULL;
    se.name = NULL;
    bool arg_exception_on_no_id_or_name = false;
    try {
      t.track_screen_view(se);
    }
    catch (invalid_argument) {
      arg_exception_on_no_id_or_name = true;
    }

    REQUIRE(arg_exception_on_no_id_or_name == true);
  }

  SECTION("track timing generates a sane event") {
    MockEmitter e;
    string url = "url";
    Tracker t(url, e);

    Tracker::TimingEvent te("category", "variable", 123);
    t.track_timing(te);

    REQUIRE(e.get_added_payloads().size() == 1);
    auto payload = e.get_added_payloads()[0].get();

    // default stuff for all events
    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    REQUIRE(payload[SNOWPLOW_PLATFORM] == "srv");
    REQUIRE(payload[SNOWPLOW_APP_ID] == "");
    REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "");

    REQUIRE(payload[SNOWPLOW_EID].size() > 5);
    REQUIRE(payload[SNOWPLOW_TIMESTAMP].size() > 10);

    REQUIRE(payload.count(SNOWPLOW_TRUE_TIMESTAMP) == 0);

    map<string, string> expected;
    expected[SNOWPLOW_UT_TIMING] = "123";
    expected[SNOWPLOW_UT_CATEGORY] = "category";
    expected[SNOWPLOW_UT_VARIABLE] = "variable";
    
    SelfDescribingJson sdj(SNOWPLOW_SCHEMA_USER_TIMINGS, expected);

    string json = sdj.to_string();
    const unsigned char* c_json = (const unsigned char*)json.c_str();
    REQUIRE(payload[SNOWPLOW_UNSTRUCTURED_ENCODED] == base64_encode(c_json, json.length()));

    string label = "hello world";
    te.label = &label;
    unsigned long long ts = Utils::get_unix_epoch_ms();
    te.true_timestamp = &ts;

    t.track_timing(te);
  
    expected[SNOWPLOW_UT_LABEL] = "hello world";
    auto new_payload = e.get_added_payloads()[1].get();

    REQUIRE(new_payload[SNOWPLOW_TRUE_TIMESTAMP] == to_string(ts));
   
    SelfDescribingJson sde_w_label(SNOWPLOW_SCHEMA_USER_TIMINGS, expected);
    string json_w_label = sde_w_label.to_string();
    REQUIRE(base64_decode(new_payload[SNOWPLOW_UNSTRUCTURED_ENCODED]) == json_w_label);
  }

  SECTION("track unstruct event generates a sane event") {
    MockEmitter e;
    string url = "something";
    Tracker t(url, e);

    Tracker::SelfDescribingEvent sde(SelfDescribingJson("schema", "{ \"hello\":\"world\" }"_json));

    t.track_unstruct_event(sde);

    REQUIRE(e.get_added_payloads().size() == 1);
    auto payload = e.get_added_payloads()[0].get();

    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    REQUIRE(payload[SNOWPLOW_PLATFORM] == "srv");
    REQUIRE(payload[SNOWPLOW_APP_ID] == "");
    REQUIRE(payload[SNOWPLOW_SP_NAMESPACE] == "");

    REQUIRE(payload[SNOWPLOW_EVENT] == SNOWPLOW_EVENT_UNSTRUCTURED);
    REQUIRE(payload[SNOWPLOW_TIMESTAMP].size() > 10);
    REQUIRE(payload[SNOWPLOW_EID].size() > 5);

    REQUIRE(payload.count(SNOWPLOW_TRUE_TIMESTAMP) == 0);

    string json = sde.event.to_string();
    const unsigned char* str = (const unsigned char*)json.c_str();
    REQUIRE(payload[SNOWPLOW_UNSTRUCTURED_ENCODED] == base64_encode(str, json.length()));
  }
}
