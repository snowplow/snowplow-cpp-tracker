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

#include "payload.hpp"

void Payload::add(string key, string value) {
  if (!key.empty() && !value.empty()) {
    m_pairs[key] = value;
  }
}

void Payload::add_map(map<string, string> pairs) {
  map<string, string>::iterator it;
  for (it = pairs.begin(); it != pairs.end(); it++) {
    add(it->first, it->second);
  }
}

void Payload::add_payload(Payload p) {
  add_map(p.get());
}

// TODO: Add base64 encoding capability
void Payload::add_json(json j, bool base64Encode, string encoded, string not_encoded) {
  if (base64Encode) {
    add(encoded, j.dump());
  } else {
    add(not_encoded, j.dump());
  }
}
