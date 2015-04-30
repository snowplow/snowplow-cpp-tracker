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

// --- Static Singleton Access

Tracker *Tracker::m_instance = 0;
mutex Tracker::m_tracker_get;

Tracker *Tracker::init(Emitter &emitter, Subject *subject, ClientSession *client_session, string *platform, string *app_id, 
  string *name_space, bool *use_base64, bool *desktop_context) {

  lock_guard<mutex> guard(m_tracker_get);
  if (!m_instance) {
    m_instance = new Tracker(emitter, subject, client_session, platform, app_id, name_space, use_base64, desktop_context);
  }
  return m_instance;
}

Tracker *Tracker::instance() {
  lock_guard<mutex> guard(m_tracker_get);
  if (!m_instance) {
    throw runtime_error("FATAL: Tracker must be initialized first.");
  }
  return m_instance;
}

void Tracker::close() {
  lock_guard<mutex> guard(m_tracker_get);
  if (m_instance) {
    delete(m_instance);
  }
  m_instance = 0;
}

// --- Constructor & Destructor

Tracker::Tracker(Emitter &emitter, Subject *subject, ClientSession *client_session, string *platform, string *app_id, 
  string *name_space, bool *use_base64, bool *desktop_context) : m_emitter(emitter), m_client_session(client_session) {

  this->m_subject = subject;
  this->m_platform = (platform != NULL ? *platform : "srv");
  this->m_app_id = (app_id != NULL ? *app_id : "");
  this->m_namespace = (name_space != NULL ? *name_space : "");
  this->m_use_base64 = (use_base64 != NULL ? *use_base64 : true);
  this->m_desktop_context = (desktop_context != NULL ? *desktop_context : true);

  // Start daemon threads
  this->start();
}

Tracker::~Tracker() {
  this->stop();
}

// --- Controls

void Tracker::start() {
  this->m_emitter.start();
  if (this->m_client_session) {
    this->m_client_session->start();
  }
}

void Tracker::stop() {
  this->m_emitter.stop();
  if (this->m_client_session) {
    this->m_client_session->stop();
  }
}

void Tracker::flush() {
  this->m_emitter.flush();
}

// --- Setters

void Tracker::set_subject(Subject *subject) {
  this->m_subject = subject;
}

// --- Event Tracking

void Tracker::track(Payload payload, const string & event_id, vector<SelfDescribingJson> & contexts) { 
  // Add standard KV Pairs
  payload.add(SNOWPLOW_TRACKER_VERSION, SNOWPLOW_TRACKER_VERSION_LABEL);
  payload.add(SNOWPLOW_PLATFORM, this->m_platform);
  payload.add(SNOWPLOW_APP_ID, this->m_app_id);
  payload.add(SNOWPLOW_SP_NAMESPACE, this->m_namespace);

  // Add Subject KV Pairs
  if (this->m_subject != NULL) {
    payload.add_map(this->m_subject->get_map());
  }

  // Add Client Session if available
  if (this->m_client_session) {
    contexts.push_back(this->m_client_session->get_session_context(event_id));
  }

  // Add Desktop Context if available
  if (this->m_desktop_context) {
    contexts.push_back(Utils::get_desktop_context());
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
  if (se.action == "") {
    throw invalid_argument("Action is required"); 
  }
  if (se.category == "") { 
    throw invalid_argument("Category is required"); 
  }

  Payload p;
  p.add(SNOWPLOW_EVENT, SNOWPLOW_EVENT_STRUCTURED);
  p.add(SNOWPLOW_SE_ACTION, se.action);
  p.add(SNOWPLOW_SE_CATEGORY, se.category);

  if (se.label != NULL) {
    p.add(SNOWPLOW_SE_LABEL, *se.label);
  }
  if (se.property != NULL) {
    p.add(SNOWPLOW_SE_PROPERTY, *se.property);
  }
  if (se.value != NULL) {
    p.add(SNOWPLOW_SE_VALUE, to_string(*se.value));
  }

  p.add(SNOWPLOW_TIMESTAMP, to_string(se.timestamp));
  p.add(SNOWPLOW_EID, se.event_id);

  if (se.true_timestamp != NULL) {
    p.add(SNOWPLOW_TRUE_TIMESTAMP, to_string(*se.true_timestamp));
  }

  track(p, se.event_id, se.contexts);
}

void Tracker::track_screen_view(Tracker::ScreenViewEvent sve) {
  if (sve.name == NULL && sve.id == NULL) {
    throw invalid_argument("Either name or id field must be set");
  }

  json data;

  if (sve.id != NULL) {
    data[SNOWPLOW_SV_ID] = *sve.id;
  }
  if (sve.name != NULL) {
    data[SNOWPLOW_SV_NAME] = *sve.name;
  }

  SelfDescribingJson sdj = SelfDescribingJson(SNOWPLOW_SCHEMA_SCREEN_VIEW, data);

  SelfDescribingEvent sde(sdj);
  sde.event_id = sve.event_id;
  sde.timestamp = sve.timestamp;
  sde.true_timestamp = sve.true_timestamp;
  sde.contexts = sve.contexts;

  track_self_describing_event(sde);
}

void Tracker::track_timing(TimingEvent te) {
  if (te.category == "") {
    throw invalid_argument("Category is required"); 
  }
  if (te.variable == "") { 
    throw invalid_argument("Variable is required"); 
  }

  json data;
  data[SNOWPLOW_UT_CATEGORY] = te.category;
  data[SNOWPLOW_UT_VARIABLE] = te.variable;
  data[SNOWPLOW_UT_TIMING] = te.timing;

  if (te.label != NULL) {
    data[SNOWPLOW_UT_LABEL] = *te.label;
  }

  SelfDescribingJson sdj = SelfDescribingJson(SNOWPLOW_SCHEMA_USER_TIMINGS, data);

  SelfDescribingEvent sde(sdj);
  sde.event_id = te.event_id;
  sde.timestamp = te.timestamp;
  sde.true_timestamp = te.true_timestamp;
  sde.contexts = te.contexts;

  track_self_describing_event(sde);
}

void Tracker::track_self_describing_event(SelfDescribingEvent sde) {
  Payload p;
  p.add(SNOWPLOW_EVENT, SNOWPLOW_EVENT_UNSTRUCTURED);
  p.add(SNOWPLOW_TIMESTAMP, to_string(sde.timestamp));
  p.add(SNOWPLOW_EID, sde.event_id);

  SelfDescribingJson sdj(SNOWPLOW_SCHEMA_UNSTRUCT_EVENT, sde.event.get());
  p.add_json(sdj.get(), this->m_use_base64, SNOWPLOW_UNSTRUCTURED_ENCODED, SNOWPLOW_UNSTRUCTURED);

  if (sde.true_timestamp != NULL) {
    p.add(SNOWPLOW_TRUE_TIMESTAMP, to_string(*sde.true_timestamp));
  }

  track(p, sde.event_id, sde.contexts);
}

// --- Event Builders

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
