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
using std::to_string;

// --- Constructor & Destructor

Tracker::Tracker(const TrackerConfiguration &tracker_config, shared_ptr<Emitter> emitter, shared_ptr<Subject> subject, shared_ptr<ClientSession> client_session) :
  Tracker(
    move(emitter),
    move(subject),
    move(client_session),
    tracker_config.get_platform(),
    tracker_config.get_app_id(),
    tracker_config.get_namespace(),
    tracker_config.get_use_base64(),
    tracker_config.get_desktop_context()
  ) {
}

Tracker::Tracker(shared_ptr<Emitter> emitter, shared_ptr<Subject> subject, shared_ptr<ClientSession> client_session, const string &platform, const string &app_id,
                 const string &name_space, bool use_base64, bool desktop_context) {
  this->m_emitter = move(emitter);
  this->m_client_session = move(client_session);
  this->m_subject = move(subject);
  this->m_platform = platform;
  this->m_app_id = app_id;
  this->m_namespace = name_space;
  this->m_use_base64 = use_base64;
  this->m_desktop_context = desktop_context;

  // Start daemon threads
  this->start();
}

Tracker::~Tracker() {
  this->stop();
}

// --- Controls

void Tracker::start() {
  this->m_emitter->start();
}

void Tracker::stop() {
  this->m_emitter->stop();
}

void Tracker::flush() {
  this->m_emitter->flush();
}

// --- Setters

void Tracker::set_subject(shared_ptr<Subject> subject) {
  this->m_subject = move(subject);
}

// --- Event Tracking

string Tracker::track(const Event &event) {
  EventPayload payload = event.get_payload(m_use_base64);
  vector<SelfDescribingJson> context = event.get_context();
  auto event_subject = event.get_subject();

  // Add standard KV Pairs
  payload.add(SNOWPLOW_TRACKER_VERSION, SNOWPLOW_TRACKER_VERSION_LABEL);
  payload.add(SNOWPLOW_PLATFORM, this->m_platform);
  payload.add(SNOWPLOW_APP_ID, this->m_app_id);
  payload.add(SNOWPLOW_SP_NAMESPACE, this->m_namespace);

  // Add Subject KV Pairs
  if (this->m_subject) {
    payload.add_map(this->m_subject->get_map());
  }

  // Add event subject pairs
  if (event_subject) {
    payload.add_map(event_subject->get_map());
  }

  // Add Client Session if available
  if (this->m_client_session) {
    context.push_back(this->m_client_session->update_and_get_session_context(payload.get_event_id(), payload.get_timestamp()));
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
  this->m_emitter->add(payload);

  return payload.get_event_id();
}
