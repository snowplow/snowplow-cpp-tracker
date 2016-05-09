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
#include "../emitter.hpp"

#include <iostream>
#include <chrono>
#include <thread>

TEST_CASE("emitter") {
  Emitter emitter("e8b9d131.ngrok.io", Emitter::Strategy::ASYNC, Emitter::Method::GET, Emitter::Protocol::HTTP, 500, "test.db");
  emitter.start();

  Payload payload;
  payload.add("e", "pv");
  payload.add("tv", "cpp-0.1.0");
  payload.add("p", "srv");
  payload.add("dtm", std::to_string(Utils::get_unix_epoch_ms()));
  emitter.add(payload);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  emitter.stop();
}
