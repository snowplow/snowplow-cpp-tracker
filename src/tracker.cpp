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

#include "tracker.hpp"

Tracker::Tracker(string & url, Emitter & e) : m_emitter(e), m_subject() {
  // set defaults
  this->m_has_subject = false;
  this->m_platform = "srv";
  this->m_app_id = "";
  this->m_use_base64 = true;
  this->m_namespace = "";

  // start the emitter daemon if it's not started already
  e.start();
}

void Tracker::track(Payload payload, vector<SelfDescribingJson> & contexts) { 
  // Add standard KV Pairs
  payload.add(SNOWPLOW_TRACKER_VERSION, SNOWPLOW_TRACKER_VERSION_LABEL);
  payload.add(SNOWPLOW_PLATFORM, this->m_platform);
  payload.add(SNOWPLOW_APP_ID, this->m_app_id);
  payload.add(SNOWPLOW_SP_NAMESPACE, this->m_namespace);

  // Add Subject KV Pairs
  if (this->m_has_subject) {
    payload.add_map(m_subject.get_map());
  }

  // Build the final context and add it to the payload
  if (contexts.size() > 0) {
    json context_data_array;
    for (int i = 0; i < contexts.size(); ++i) {
      context_data_array.push_back(contexts[i].get());
    }
    SelfDescribingJson context_json(SNOWPLOW_SCHEMA_CONTEXTS, context_data_array);
    payload.add_json(context_json.get(), m_use_base64, SNOWPLOW_CONTEXT_ENCODED, SNOWPLOW_CONTEXT);
  }

  // Add the event to the Emitter
  this->m_emitter.add(payload);
}

void Tracker::track_struct_event(StructuredEvent se) {
  if (se.action == "") { throw invalid_argument("Action is required"); }
  if (se.category == "") { throw invalid_argument("Category is required"); }

  Payload p;
  p.add(SNOWPLOW_EVENT, SNOWPLOW_EVENT_STRUCTURED);
  p.add(SNOWPLOW_SE_ACTION, se.action);
  p.add(SNOWPLOW_SE_CATEGORY, se.category);
  p.add(SNOWPLOW_TIMESTAMP, to_string(se.timestamp));

  if (se.true_timestamp != NULL) {
    p.add(SNOWPLOW_TRUE_TIMESTAMP, to_string(*se.true_timestamp));
  }

  p.add(SNOWPLOW_EID, se.event_id);

  if (se.label != NULL) {
    p.add(SNOWPLOW_SE_LABEL, *se.label);
  }

  if (se.property != NULL) {
    p.add(SNOWPLOW_SE_PROPERTY, *se.property);
  }

  if (se.value != NULL) {
    p.add(SNOWPLOW_SE_VALUE, to_string(*se.value));
  }

  track(p, se.contexts);
}

void Tracker::track_screen_view(Tracker::ScreenViewEvent sve) {

  if (sve.name == NULL && sve.id == NULL) {
    throw invalid_argument("Either name or id field must be set");
  }

  Payload p;

  p.add(SNOWPLOW_EID, sve.event_id);
  p.add(SNOWPLOW_TIMESTAMP, sve.event_id);

  if (sve.id != NULL) {
    p.add(SNOWPLOW_SV_ID, *sve.id);
  }

  if (sve.name != NULL) {
    p.add(SNOWPLOW_SV_NAME, *sve.name);
  }

  if (sve.true_timestamp != NULL) {
    p.add(SNOWPLOW_TRUE_TIMESTAMP, to_string(*sve.true_timestamp));
  }

  track(p, sve.contexts);
}

void Tracker::track_timing(TimingEvent te) {
  Payload p;

  map<string, string> m;
  m[SNOWPLOW_UT_CATEGORY] = te.category;
  m[SNOWPLOW_UT_VARIABLE] = te.variable;
  m[SNOWPLOW_UT_TIMING] = to_string(te.timing);

  if (te.label != NULL) {
    m[SNOWPLOW_UT_LABEL] = *te.label;
  }

  auto sdj = SelfDescribingJson(SNOWPLOW_SCHEMA_USER_TIMINGS, m);
  p.add_json(sdj.get(), this->m_use_base64, SNOWPLOW_UNSTRUCTURED_ENCODED, SNOWPLOW_UNSTRUCTURED);

  p.add(SNOWPLOW_EID, te.event_id);
  p.add(SNOWPLOW_TIMESTAMP, to_string(te.timestamp));

  if (te.true_timestamp != NULL) {
    p.add(SNOWPLOW_TRUE_TIMESTAMP, to_string(*te.true_timestamp));
  }
  
  track(p, te.contexts);
}

void Tracker::track_unstruct_event(SelfDescribingEvent sde) {
  Payload p;
  p.add(SNOWPLOW_EVENT, SNOWPLOW_EVENT_UNSTRUCTURED);
  p.add(SNOWPLOW_TIMESTAMP, to_string(sde.timestamp));
  p.add(SNOWPLOW_EID, sde.event_id);

  p.add_json(sde.event.get(), this->m_use_base64, SNOWPLOW_UNSTRUCTURED_ENCODED, SNOWPLOW_UNSTRUCTURED);

  track(p, sde.contexts);
}

void Tracker::flush()
{
  m_emitter.flush();
}

void Tracker::close()
{
  m_emitter.stop();
}

Tracker::~Tracker()
{
  close();
}

Tracker::StructuredEvent::StructuredEvent(string category, string action) {
  this->category = category;
  this->action = action;
  this->contexts = vector<SelfDescribingJson>();
  this->event_id = Utils::get_uuid4();
  this->timestamp = Utils::get_unix_epoch_ms();
  this->true_timestamp = NULL;
  this->label = NULL;
  this->property = NULL;
  this->value = NULL;
}

Tracker::SelfDescribingEvent::SelfDescribingEvent(SelfDescribingJson event): event(event) {
  this->event_id = Utils::get_uuid4();
  this->timestamp = Utils::get_unix_epoch_ms();
  this->true_timestamp = NULL;
  this->contexts = vector<SelfDescribingJson>();
}

Tracker::ScreenViewEvent::ScreenViewEvent() {
  this->contexts = vector<SelfDescribingJson>();
  this->event_id = Utils::get_uuid4();
  this->timestamp = Utils::get_unix_epoch_ms();
  this->true_timestamp = NULL;
  this->id = NULL;
  this->name = NULL;
}

Tracker::TimingEvent::TimingEvent(string category, string variable, unsigned long long timing) {
  this->category = category;
  this->variable = variable;
  this->timestamp = Utils::get_unix_epoch_ms();
  this->true_timestamp = NULL;
  this->timing = timing;
  this->contexts = vector<SelfDescribingJson>();
  this->event_id = Utils::get_uuid4();
  this->label = NULL;
}
