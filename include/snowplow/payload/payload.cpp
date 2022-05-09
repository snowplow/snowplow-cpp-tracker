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

#include "payload.hpp"
#include "../utils.hpp"

using namespace snowplow;
using std::to_string;

Payload::~Payload() {
  this->m_pairs.clear();
}

void Payload::add(const string &key, const string &value) {
  if (!key.empty() && !value.empty()) {
    this->m_pairs[key] = value;
  }
}

void Payload::add_map(map<string, string> pairs) {
  map<string, string>::iterator it;
  for (it = pairs.begin(); it != pairs.end(); it++) {
    this->add(it->first, it->second);
  }
}

void Payload::add_payload(const Payload &p) {
  this->add_map(p.get());
}

void Payload::add_json(const json &j, bool base64Encode, const string &encoded, const string &not_encoded) {
  if (base64Encode) {
    string json_str = j.dump();
    this->add(encoded, base64_encode((const unsigned char *)json_str.c_str(), json_str.length()));
  } else {
    this->add(not_encoded, j.dump());
  }
}

map<string, string> Payload::get() const {
  return m_pairs;
}
