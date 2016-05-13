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

#ifndef TRACKER_H
#define TRACKER_H

#include <string>
#include "emitter.hpp"
#include "subject.hpp"
#include "self_describing_json.hpp"

using namespace std;

class Tracker {
private:
  Emitter & m_emitter;
  Subject m_subject;
  bool m_has_subject;
  string m_namespace;
  string m_app_id;
  string m_platform;
  bool m_use_base64;

public:
  void track(Payload p, vector<SelfDescribingJson> & contexts);
  Tracker(string & url, Emitter & e);
  ~Tracker();
  void flush();
  void close();
};

#endif
