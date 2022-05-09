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

#include "event.hpp"
#include "../utils.hpp"

using namespace snowplow;
using std::to_string;
using std::invalid_argument;
using std::move;

Event::Event() {
  this->m_true_timestamp = NULL;
}

// --- Getters

EventPayload Event::get_payload(bool use_base64) const {
  EventPayload p = get_custom_event_payload(use_base64);

  if (!p.get().count(SNOWPLOW_EVENT)) {
    throw invalid_argument("Missing event type");
  }

  auto *true_timestamp = get_true_timestamp();
  if (true_timestamp != NULL) {
    p.add(SNOWPLOW_TRUE_TIMESTAMP, to_string(*true_timestamp));
  }

  return p;
}

EventPayload Event::get_self_describing_event_payload(const SelfDescribingJson &event, bool use_base64) const {
  EventPayload p;
  p.add(SNOWPLOW_EVENT, SNOWPLOW_EVENT_SELF_DESCRIBING);

  SelfDescribingJson sdj(SNOWPLOW_SCHEMA_UNSTRUCT_EVENT, event.get());
  p.add_json(sdj.get(), use_base64, SNOWPLOW_UNSTRUCTURED_ENCODED, SNOWPLOW_UNSTRUCTURED);

  return p;
}

vector<SelfDescribingJson> Event::get_context() const {
  return m_context;
}

unsigned long long *Event::get_true_timestamp() const {
  return m_true_timestamp;
}

shared_ptr<Subject> Event::get_subject() const {
  return m_subject;
}

// --- Setters

void Event::set_true_timestamp(unsigned long long *true_timestamp) {
  m_true_timestamp = true_timestamp;
}

void Event::set_context(const vector<SelfDescribingJson> &context) {
  m_context = context;
}

void Event::set_subject(shared_ptr<Subject> subject) {
  m_subject = move(subject);
}
