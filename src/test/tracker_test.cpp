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

    /*
     payload.add(SNOWPLOW_TRACKER_VERSION, SNOWPLOW_TRACKER_VERSION_LABEL);
  payload.add(SNOWPLOW_PLATFORM, this->m_platform);
  payload.add(SNOWPLOW_APP_ID, this->m_app_id);
  payload.add(SNOWPLOW_SP_NAMESPACE, this->m_namespace);

    */

    REQUIRE(payload[SNOWPLOW_TRACKER_VERSION] == SNOWPLOW_TRACKER_VERSION_LABEL);
    //REQUIRE(payload[SNOWPLOW_PLATFORM] == SNOWPLOW_PL);

  }

}
