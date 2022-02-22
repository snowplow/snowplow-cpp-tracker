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

#include <string>
#include <chrono>

#include "run.hpp"
#include "../src/tracker.hpp"
#include "../src/subject.hpp"
#include "mock_emitter.hpp"
#include "mock_client_session.hpp"
#include "mute_emitter.hpp"
#include "mock_client_session.hpp"

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::vector;

void clear_storage(const string &db_name);

void track_events() {
  Tracker *tracker = Tracker::instance();

  for (int i = 0; i < NUM_OPERATIONS; i++) {
    Tracker::TimingEvent te("timing-cat", "timing-var", 123);

    Tracker::ScreenViewEvent sve;
    string name = "Screen ID - 5asd56";
    sve.name = &name;

    Tracker::StructuredEvent se("shop", "add-to-basket");
    string property = "pcs";
    double value = 25.6;
    se.property = &property;
    se.value = &value;

    tracker->track_timing(te);
    tracker->track_screen_view(sve);
    tracker->track_struct_event(se);
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
  MockEmitter emitter(db_name);
  MockClientSession client_session(db_name);
  double time = run(emitter, client_session);
  Storage::close();
  return time;
}

double run_mocked_emitter_and_real_session(const string &db_name) {
  MockEmitter emitter(db_name);
  ClientSession client_session(db_name, 5000, 5000);
  clear_storage(db_name);
  double time = run(emitter, client_session);
  Storage::close();
  return time;
}

double run_mute_emitter_and_mocked_session(const string &db_name) {
  MuteEmitter emitter(db_name);
  MockClientSession client_session(db_name);
  clear_storage(db_name);
  double time = run(emitter, client_session);
  Storage::close();
  return time;
}

double run_mute_emitter_and_real_session(const string &db_name) {
  MuteEmitter emitter(db_name);
  ClientSession client_session(db_name, 5000, 5000);
  clear_storage(db_name);
  double time = run(emitter, client_session);
  Storage::close();
  return time;
}

void clear_storage(const string &db_name) {
  Storage *storage = Storage::init(db_name);
  storage->delete_all_event_rows();
  storage->delete_all_session_rows();
}
