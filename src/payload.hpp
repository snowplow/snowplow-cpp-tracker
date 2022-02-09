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

#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <map>
#include <string>
#include "../include/json.hpp"
#include "../include/base64.hpp"

using namespace std;
using json = nlohmann::json;

class Payload {
private:
  map<string, string> m_pairs;

public:
  ~Payload();
  void add(const string & key, const string & value);
  void add_map(map<string, string> pairs);
  void add_payload(Payload p);
  void add_json(json j, bool base64Encode, const string & encoded, const string & not_encoded);
  map<string, string> get();
};

#endif
