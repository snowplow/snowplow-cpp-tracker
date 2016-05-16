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

TEST_CASE("tracker") {

  class MockEmitter : public Emitter {
    bool started = false;
    vector<Payload> p;
  public:
    MockEmitter() : Emitter("abc123.duck", Emitter::Strategy::ASYNC, Emitter::Method::POST, Emitter::Protocol::HTTP, 0, 0, 0, "test.db") {}

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

  SECTION("page view events have appropriate defaults") {
    unsigned long long time_now = Utils::get_unix_epoch_ms();
    Tracker::PageViewEvent p("http://hello/world");
    REQUIRE(p.page_url == "http://hello/world");
    REQUIRE(p.page_title == "");
    REQUIRE(p.referrer == "");
    REQUIRE(p.timestamp > (time_now - 1000));
    REQUIRE(p.timestamp < (time_now + 1000));
    REQUIRE(p.true_timestamp == 0);
    REQUIRE(p.contexts.size() == 0);
  }

  SECTION("structured events have appropriate defaults") {
    unsigned long long time_now = Utils::get_unix_epoch_ms();
    Tracker::StructuredEvent s("category", "action");
    REQUIRE(s.category == "category");
    REQUIRE(s.action == "action");
    REQUIRE(s.contexts.size() == 0);
    REQUIRE(s.event_id.size() > 5);
    REQUIRE(s.label == "");
    REQUIRE(s.timestamp > (time_now - 1000));
    REQUIRE(s.timestamp < (time_now + 1000));
    REQUIRE(s.true_timestamp == 0);
    REQUIRE(s.value == 0.0);
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
    REQUIRE(sve.id == "");
    REQUIRE(sve.name == "");
    REQUIRE(sve.timestamp > time_now - 1000);
    REQUIRE(sve.timestamp < time_now + 1000);
    REQUIRE(sve.true_timestamp == 0);
  }

  SECTION("TimingEvents have appropriate defaults") {
    unsigned long long time_now = Utils::get_unix_epoch_ms();
    Tracker::TimingEvent t("cat", "variable");
    REQUIRE(t.category == "cat");
    REQUIRE(t.variable == "variable");
    REQUIRE(t.timestamp > time_now - 1000);
    REQUIRE(t.timestamp < time_now + 1000);
    REQUIRE(t.true_timestamp == 0);
    REQUIRE(t.label == "");
    REQUIRE(t.timing == 0);
    REQUIRE(t.contexts.size() == 0);
    REQUIRE(t.event_id.size() > 5);
  }

  SECTION("EcommerceTransactionItemEvent's have appropriate defaults") {
    unsigned long long time_now = Utils::get_unix_epoch_ms();
    Tracker::EcommerceTransactionItemEvent e("cat", 99.99);
    REQUIRE(e.sku == "cat");
    REQUIRE(e.price == 99.99);
    REQUIRE(e.category == "");
    REQUIRE(e.contexts.size() == 0);
    REQUIRE(e.event_id.size() > 5);
    REQUIRE(e.name == "");
    REQUIRE(e.quantity == 1);
  }

  SECTION("EcommerceTransactionEvent's have appropriate defaults") {
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
  }
}
