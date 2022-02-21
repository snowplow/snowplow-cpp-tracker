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

#ifndef SUBJECT_H
#define SUBJECT_H

#include <map>
#include <string>
#include "payload.hpp"
#include "constants.hpp"

using std::map;
using std::string;

class Subject {
private:
  Payload m_payload;

public:
  void set_user_id(const string & user_id);
  void set_screen_resolution(int width, int height);
  void set_viewport(int width, int height);
  void set_color_depth(int depth);
  void set_timezone(const string & timezone);
  void set_language(const string & language);
  void set_useragent(const string & user_agent);
  map<string, string> get_map();
};

#endif
