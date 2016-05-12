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
#include <map>
#include <cctype>
#include <iomanip>
#include <chrono>
#include "payload.hpp"
#include "vendored/json.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#include <Windows.h>
#include <rpc.h>

#pragma comment (lib, "Rpcrt4.lib")

#elif defined(__APPLE__)

#include <CoreFoundation/CoreFoundation.h>

#endif

using namespace std;
using json = nlohmann::json;

class Utils {
public:
  static string get_uuid4();
  static string int_list_to_string(list<int>* int_list, const string & delimiter);
  static string map_to_query_string(map<string, string> m);
  static string url_encode(string value);
  static string serialize_payload(Payload payload);
  static Payload deserialize_json_str(const string & json_str);
  static unsigned long long get_unix_epoch_ms();
};

#endif
