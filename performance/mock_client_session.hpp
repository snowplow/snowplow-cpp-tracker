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

#include "../src/client_session.hpp"

using std::string;

class MockClientSession : public ClientSession {
public:
   MockClientSession(const string &db_name) : ClientSession(db_name, 5000, 5000, 2500) {}

   void start() {}
   void stop() {}
   void set_is_background(bool is_background) {}
   bool get_is_background() { return false; }
   SelfDescribingJson get_session_context(const string &event_id) { return SelfDescribingJson("schema", json()); }
};

#endif
