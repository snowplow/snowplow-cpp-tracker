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

#ifndef TRACKER_H
#define TRACKER_H

#include <string>
#include "emitter.hpp"
#include "subject.hpp"
#include "client_session.hpp"
#include "self_describing_json.hpp"

using namespace std;

class Tracker {
public:
  static Tracker *init(Emitter &emitter, Subject *subject, ClientSession *client_session, string *platform, 
    string *app_id, string *name_space, bool *use_base64, bool *desktop_context);
  static Tracker *instance();
  static void close();

  class StructuredEvent {
  public:
    string category; // required
    string action; // required
    string *label;
    string *property;
    double *value;
    unsigned long long timestamp;
    string event_id;
    unsigned long long *true_timestamp;
    vector<SelfDescribingJson> contexts;
    StructuredEvent(string category, string action);
  };

  class SelfDescribingEvent {
  public:
    SelfDescribingJson event; // required
    unsigned long long timestamp;
    string event_id;
    unsigned long long *true_timestamp;
    vector<SelfDescribingJson> contexts;
    SelfDescribingEvent(SelfDescribingJson event);
  };

  class ScreenViewEvent {
  public:
    string *name;
    string *id;
    unsigned long long timestamp;
    string event_id;
    unsigned long long *true_timestamp;
    vector<SelfDescribingJson> contexts;
    ScreenViewEvent();
  };

  class TimingEvent {
  public:
    string category; // required
    string variable; // required
    unsigned long long timing; // required
    string *label;
    unsigned long long timestamp;
    string event_id;
    unsigned long long *true_timestamp;
    vector<SelfDescribingJson> contexts;
    TimingEvent(string category, string variable, unsigned long long timing);
  };

  void start();
  void stop();
  void flush();

  void set_subject(Subject *subject);

  void track(Payload p, const string & event_id, vector<SelfDescribingJson> &contexts);
  void track_struct_event(StructuredEvent);
  void track_screen_view(ScreenViewEvent);
  void track_timing(TimingEvent);
  void track_self_describing_event(SelfDescribingEvent);

private:
  static Tracker *m_instance;
  static mutex m_tracker_get;

  Tracker(Emitter &emitter, Subject *subject, ClientSession *client_session, string *platform, 
    string *app_id, string *name_space, bool *use_base64, bool *desktop_context);
  ~Tracker();

  Emitter &m_emitter;
  Subject *m_subject;
  ClientSession *m_client_session;
  string m_namespace;
  string m_app_id;
  string m_platform;
  bool m_use_base64;
  bool m_desktop_context;
};

#endif
