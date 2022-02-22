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

#include "subject.hpp"

using namespace snowplow;

void Subject::set_user_id(const string &user_id) {
  this->m_payload.add(SNOWPLOW_UID, user_id);
}

void Subject::set_screen_resolution(int width, int height) {
  string res = std::to_string(width) + "x" + std::to_string(height);
  this->m_payload.add(SNOWPLOW_RESOLUTION, res);
}

void Subject::set_viewport(int width, int height) {
  string vport = std::to_string(width) + "x" + std::to_string(height);
  this->m_payload.add(SNOWPLOW_VIEWPORT, vport);
}

void Subject::set_color_depth(int depth) {
  this->m_payload.add(SNOWPLOW_COLOR_DEPTH, std::to_string(depth));
}

void Subject::set_timezone(const string &timezone) {
  this->m_payload.add(SNOWPLOW_TIMEZONE, timezone);
}

void Subject::set_language(const string &language) {
  this->m_payload.add(SNOWPLOW_LANGUAGE, language);
}

void Subject::set_useragent(const string &useragent) {
  this->m_payload.add(SNOWPLOW_USERAGENT, useragent);
}

map<string, string> Subject::get_map() {
  return this->m_payload.get();
}
