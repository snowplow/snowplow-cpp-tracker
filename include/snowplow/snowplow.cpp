/*
Copyright (c) 2023 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#include "snowplow.hpp"

using namespace snowplow;

using std::make_shared;
using std::lock_guard;

map<string, shared_ptr<Tracker>> Snowplow::m_trackers;
mutex Snowplow::m_tracker_get;
shared_ptr<Tracker> Snowplow::m_default_tracker = nullptr;

shared_ptr<Tracker> Snowplow::create_tracker(const string &name_space, const string &collector_url, Method method, const string &db_name, shared_ptr<Subject> subject, bool session_tracking) {
  auto storage = make_shared<SqliteStorage>(db_name);
  return session_tracking ?
      create_tracker(name_space, collector_url, method, storage, storage, subject) :
      create_tracker(name_space, collector_url, method, storage, subject);
}

shared_ptr<Tracker> Snowplow::create_tracker(const string &name_space, const string &collector_url, Method method, shared_ptr<EventStore> event_store, shared_ptr<Subject> subject) {
  NetworkConfiguration network_config(collector_url, method);
  return create_tracker(
      TrackerConfiguration(name_space),
      network_config,
      EmitterConfiguration(std::move(event_store)),
      std::move(subject));
}

shared_ptr<Tracker> Snowplow::create_tracker(const string &name_space, const string &collector_url, Method method, shared_ptr<EventStore> event_store, shared_ptr<SessionStore> session_store, shared_ptr<Subject> subject) {
  NetworkConfiguration network_config(collector_url, method);
  SessionConfiguration session_config(std::move(session_store));
  EmitterConfiguration emitter_config(std::move(event_store));
  return create_tracker(
      TrackerConfiguration(name_space),
      network_config,
      emitter_config,
      session_config,
      std::move(subject));
}

shared_ptr<Tracker> Snowplow::create_tracker(const TrackerConfiguration &tracker_config, NetworkConfiguration &network_config, const EmitterConfiguration &emitter_config, shared_ptr<Subject> subject) {
  auto emitter = make_shared<Emitter>(network_config, emitter_config);
  auto tracker = make_shared<Tracker>(tracker_config, std::move(emitter), std::move(subject));
  register_tracker(tracker);
  return tracker;
}

shared_ptr<Tracker> Snowplow::create_tracker(const TrackerConfiguration &tracker_config, NetworkConfiguration &network_config, EmitterConfiguration &emitter_config, SessionConfiguration &session_config, shared_ptr<Subject> subject) {
  if (emitter_config.get_db_name() != "" && emitter_config.get_db_name() == session_config.get_db_name()) {
    auto storage = make_shared<SqliteStorage>(emitter_config.get_db_name());
    emitter_config.set_event_store(storage);
    session_config.set_session_store(storage);
  }
  auto emitter = make_shared<Emitter>(network_config, emitter_config);
  auto client_session = make_shared<ClientSession>(session_config);
  auto tracker = make_shared<Tracker>(tracker_config, std::move(emitter), std::move(subject), std::move(client_session));
  register_tracker(tracker);
  return tracker;
}

void Snowplow::register_tracker(shared_ptr<Tracker> tracker) {
  lock_guard<mutex> guard(m_tracker_get);

  string ns = tracker->get_namespace();
  if (m_trackers.count(ns)) {
    throw std::runtime_error("Tracker with the namespace already exists.");
  }

  m_trackers.insert({ns, tracker});

  if (!m_default_tracker) {
    m_default_tracker = tracker;
  }
}

shared_ptr<Tracker> Snowplow::get_tracker(const string &name_space) {
  lock_guard<mutex> guard(m_tracker_get);

  if (m_trackers.count(name_space)) {
    return m_trackers.at(name_space);
  }

  return nullptr;
}

shared_ptr<Tracker> Snowplow::get_default_tracker() {
  lock_guard<mutex> guard(m_tracker_get);

  return m_default_tracker;
}

void Snowplow::set_default_tracker(shared_ptr<Tracker> tracker) {
  lock_guard<mutex> guard(m_tracker_get);

  string ns = tracker->get_namespace();
  if (m_trackers.count(ns) == 0) {
    m_trackers.insert({ns, tracker});
  }

  m_default_tracker = tracker;
}

bool Snowplow::remove_tracker(shared_ptr<Tracker> tracker) {
  return remove_tracker(tracker->get_namespace());
}

bool Snowplow::remove_tracker(const string &name_space) {
  lock_guard<mutex> guard(m_tracker_get);
  int num_removed = int(m_trackers.erase(name_space));

  if (m_default_tracker && m_default_tracker->get_namespace() == name_space) {
    m_default_tracker = nullptr;
  }

  return num_removed > 0;
}
