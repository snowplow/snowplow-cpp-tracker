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

#include "timing_event.hpp"

using namespace snowplow;
using std::invalid_argument;

TimingEvent::TimingEvent(const string &category, const string &variable, unsigned long long timing) {
  this->category = category;
  this->variable = variable;
  this->timing = timing;
  this->label = NULL;
}

EventPayload TimingEvent::get_custom_event_payload(bool use_base64) const {
  if (category == "") {
    throw invalid_argument("Category is required");
  }
  if (variable == "") {
    throw invalid_argument("Variable is required");
  }

  json data;
  data[SNOWPLOW_UT_CATEGORY] = category;
  data[SNOWPLOW_UT_VARIABLE] = variable;
  data[SNOWPLOW_UT_TIMING] = timing;

  if (label != NULL) {
    data[SNOWPLOW_UT_LABEL] = *label;
  }

  SelfDescribingJson sdj = SelfDescribingJson(SNOWPLOW_SCHEMA_USER_TIMINGS, data);

  return get_self_describing_event_payload(sdj, use_base64);
}
