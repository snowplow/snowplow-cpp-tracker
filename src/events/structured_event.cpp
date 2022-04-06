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

#include "structured_event.hpp"

using namespace snowplow;
using std::invalid_argument;
using std::to_string;

StructuredEvent::StructuredEvent(const string &category, const string &action) {
  this->category = category;
  this->action = action;
  this->label = NULL;
  this->property = NULL;
  this->value = NULL;
}

EventPayload StructuredEvent::get_custom_event_payload(bool use_base64) const {
  if (action == "") {
    throw invalid_argument("Action is required");
  }
  if (category == "") {
    throw invalid_argument("Category is required");
  }

  EventPayload p;
  p.add(SNOWPLOW_EVENT, SNOWPLOW_EVENT_STRUCTURED);
  p.add(SNOWPLOW_SE_ACTION, action);
  p.add(SNOWPLOW_SE_CATEGORY, category);

  if (label != NULL) {
    p.add(SNOWPLOW_SE_LABEL, *label);
  }
  if (property != NULL) {
    p.add(SNOWPLOW_SE_PROPERTY, *property);
  }
  if (value != NULL) {
    p.add(SNOWPLOW_SE_VALUE, to_string(*value));
  }

  return p;
}
