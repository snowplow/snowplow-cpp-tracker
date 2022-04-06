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

#include "tracker.hpp"

using namespace snowplow;
using std::invalid_argument;
using std::lock_guard;
using std::runtime_error;
using std::to_string;

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
    delete (m_instance);
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
}

void Tracker::stop() {
  this->m_emitter.stop();
}

void Tracker::flush() {
  this->m_emitter.flush();
}

// --- Setters

void Tracker::set_subject(Subject *subject) {
  this->m_subject = subject;
}

// --- Event Tracking

vector<string> Tracker::track(const Event &event) {
  EventPayload payload = event.get_payload(m_use_base64);
  vector<SelfDescribingJson> context = event.get_context();

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
    context.push_back(this->m_client_session->update_and_get_session_context(payload.get_event_id()));
  }

  // Add Desktop Context if available
  if (this->m_desktop_context) {
    context.push_back(Utils::get_desktop_context());
  }

  // Build the final context and add it to the payload
  if (context.size() > 0) {
    json context_data_array;
    for (int i = 0; i < context.size(); ++i) {
      context_data_array.push_back(context[i].get());
    }
    SelfDescribingJson context_json(SNOWPLOW_SCHEMA_CONTEXTS, context_data_array);
    payload.add_json(context_json.get(), m_use_base64, SNOWPLOW_CONTEXT_ENCODED, SNOWPLOW_CONTEXT);
  }

  // Add the event to the Emitter
  this->m_emitter.add(payload);

  // Return a vector with the tracked event ID
  vector<string> event_ids;
  event_ids.push_back(payload.get_event_id());
  return event_ids;
}
