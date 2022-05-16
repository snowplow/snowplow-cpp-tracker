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

#ifndef MOCK_CLIENT_SESSION_H
#define MOCK_CLIENT_SESSION_H

#include "../include/snowplow/snowplow.hpp"

using snowplow::ClientSession;
using snowplow::SelfDescribingJson;
using snowplow::SessionStore;
using std::string;
using std::shared_ptr;
using std::move;

class MockClientSession : public ClientSession {
public:
   MockClientSession(shared_ptr<SessionStore> session_store) : ClientSession(move(session_store), 5000, 5000) {}

   void set_is_background(bool is_background) {}
   bool get_is_background() { return false; }
   SelfDescribingJson update_and_get_session_context(const string &event_id) { return SelfDescribingJson("schema", json()); }
};

#endif
