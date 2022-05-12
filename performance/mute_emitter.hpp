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

#ifndef MUTE_EMITTER_H
#define MUTE_EMITTER_H

#include "../src/snowplow.hpp"

using snowplow::Emitter;
using snowplow::EventStore;
using snowplow::Method;
using snowplow::Protocol;
using std::string;
using std::shared_ptr;
using std::move;

class MuteEmitter : public Emitter {
public:
   MuteEmitter(shared_ptr<EventStore> event_store) : Emitter(move(event_store), "127.0.0.1:9090", Method::POST, Protocol::HTTP, 500, 52000, 52000) {}

   void start() {}
   void stop() {}
};

#endif
