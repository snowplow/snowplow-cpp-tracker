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

#include "event_payload.hpp"
#include "../utils.hpp"

using namespace snowplow;
using std::to_string;

EventPayload::EventPayload() {
  this->m_event_id = Utils::get_uuid4();
  this->m_timestamp = Utils::get_unix_epoch_ms();

  add(SNOWPLOW_TIMESTAMP, to_string(m_timestamp));
  add(SNOWPLOW_EID, m_event_id);
}

string EventPayload::get_event_id() const {
  return m_event_id;
}

unsigned long long EventPayload::get_timestamp() const {
  return m_timestamp;
}
