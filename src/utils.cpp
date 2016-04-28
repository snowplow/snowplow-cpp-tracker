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

#include "utils.hpp"

string Utils::int_list_to_string(list<int>* int_list, string delimiter) {
  stringstream s;
  int i;
  list<int>::iterator it;

  int length = int_list->size();
  for (i = 0, it = int_list->begin(); it != int_list->end(); ++it, ++i) {
    s << *it;
    if (i < length - 1) {
      s << delimiter;
    }
  }

  return s.str();
}

string Utils::serialize_payload(Payload payload) {
  json j_map(payload.get());
  return j_map.dump();
}

Payload Utils::deserialize_json_str(const string json_str) {
  Payload p;
  json j = json::parse(json_str);
  
  for (json::iterator it = j.begin(); it != j.end(); ++it) {
    p.add(it.key(), it.value());
  }

  return p;
}
