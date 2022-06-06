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

#include "self_describing_event.hpp"

using namespace snowplow;

SelfDescribingEvent::SelfDescribingEvent(const SelfDescribingJson &event) : event(event) {
}

EventPayload SelfDescribingEvent::get_custom_event_payload(bool use_base64) const {
  return get_self_describing_event_payload(event, use_base64);
}
