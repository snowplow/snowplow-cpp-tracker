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

#ifndef MOCK_EMITTER_H
#define MOCK_EMITTER_H

#include "../src/emitter.hpp"

using snowplow::Emitter;
using snowplow::Payload;
using std::string;

class MockEmitter : public Emitter {
public:
   MockEmitter(const string &db_name) : Emitter("127.0.0.1:9090", Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 52000, db_name) {}

   void start() {}
   void stop() {}
   void add(Payload payload) {}
};

#endif
