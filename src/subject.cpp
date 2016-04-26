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

#include "subject.hpp"

void Subject::set_user_id(string user_id) {
  m_payload.add("uid", user_id);
}

void Subject::set_screen_resolution(int width, int height) {
  string res = std::to_string(width) + "x" + std::to_string(height);
  m_payload.add("res", res);
}

void Subject::set_viewport(int width, int height) {
  string vport = std::to_string(width) + "x" + std::to_string(height);
  m_payload.add("vp", vport);
}

void Subject::set_color_depth(int depth) {
  m_payload.add("cd", std::to_string(depth));
}

void Subject::set_timezone(string timezone) {
  m_payload.add("tz", timezone);
}

void Subject::set_language(string language) {
  m_payload.add("lang", language);
}
