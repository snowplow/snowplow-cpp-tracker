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

#include <chrono>
#include <string>

#include "../src/snowplow.hpp"
#include "mock_client_session.hpp"
#include "mock_emitter.hpp"
#include "mute_emitter.hpp"
#include "run.hpp"

using snowplow::ClientSession;
using snowplow::Emitter;
using snowplow::Subject;
using snowplow::Tracker;
using snowplow::ScreenViewEvent;
using snowplow::StructuredEvent;
using snowplow::TimingEvent;
using snowplow::SqliteStorage;
using std::vector;
using std::chrono::duration;
using std::chrono::high_resolution_clock;

void clear_storage(shared_ptr<SqliteStorage> &db_name);

void track_events() {
  Tracker *tracker = Tracker::instance();

  for (int i = 0; i < NUM_OPERATIONS; i++) {
    TimingEvent te("timing-cat", "timing-var", 123);

    ScreenViewEvent sve;
    string name = "Screen ID - 5asd56";
    sve.name = &name;

    StructuredEvent se("shop", "add-to-basket");
    string property = "pcs";
    double value = 25.6;
    se.property = &property;
    se.value = &value;

    tracker->track(te);
    tracker->track(sve);
    tracker->track(se);
  }
}

double run(Emitter &emitter, ClientSession &client_session) {
  string platform = "mob";
  string app_id = "app-id";
  string name_space = "namespace";
  bool base64 = false;
  bool desktop_context = true;

  Subject subject;
  subject.set_user_id("a-user-id");
  subject.set_screen_resolution(1920, 1080);
  subject.set_viewport(1080, 1080);
  subject.set_color_depth(32);
  subject.set_timezone("GMT");
  subject.set_language("EN");

  Tracker::init(emitter, &subject, &client_session, &platform, &app_id, &name_space, &base64, &desktop_context);

  high_resolution_clock::time_point t0 = high_resolution_clock::now();

  vector<thread> threads(NUM_THREADS);
  for (int i = 0; i < NUM_THREADS; i++) {
    threads[i] = thread(track_events);
  }
  for (int i = 0; i < NUM_THREADS; i++) {
    threads[i].join();
  }

  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  duration<double> diff = t1 - t0;
  return diff.count();
}

double run_mocked_emitter_and_mocked_session(const string &db_name) {
  auto storage = std::make_shared<SqliteStorage>(db_name);
  MockEmitter emitter(storage);
  MockClientSession client_session(storage);
  double time = run(emitter, client_session);
  return time;
}

double run_mocked_emitter_and_real_session(const string &db_name) {
  auto storage = std::make_shared<SqliteStorage>(db_name);
  MockEmitter emitter(storage);
  ClientSession client_session(storage, 5000, 5000);
  clear_storage(storage);
  double time = run(emitter, client_session);
  return time;
}

double run_mute_emitter_and_mocked_session(const string &db_name) {
  auto storage = std::make_shared<SqliteStorage>(db_name);
  MuteEmitter emitter(storage);
  MockClientSession client_session(storage);
  clear_storage(storage);
  double time = run(emitter, client_session);
  return time;
}

double run_mute_emitter_and_real_session(const string &db_name) {
  auto storage = std::make_shared<SqliteStorage>(db_name);
  MuteEmitter emitter(storage);
  ClientSession client_session(storage, 5000, 5000);
  clear_storage(storage);
  double time = run(emitter, client_session);
  return time;
}

void clear_storage(shared_ptr<SqliteStorage> &storage) {
  storage->delete_all_event_rows();
  storage->delete_session();
}
