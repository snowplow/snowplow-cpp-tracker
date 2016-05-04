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

#ifndef UTILS_H
#define UTILS_H

#include <list>
#include <string>
#include <sstream>
#include <iostream>
#include "payload.hpp"
#include "vendored/json.hpp"

using namespace std;

class Utils {
public:
  static string int_list_to_string(list<int>* int_list, const string & delimiter);
  static string serialize_payload(Payload payload);
  static Payload deserialize_json_str(const string & json_str);
};

#endif
