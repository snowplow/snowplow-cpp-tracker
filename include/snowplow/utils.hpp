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
#include <cstdint>
#include <thread>
#include "payload/self_describing_json.hpp"
#include "payload/payload.hpp"
#include "thirdparty/json.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#include <Windows.h>
#include <rpc.h>

#pragma comment (lib, "Rpcrt4.lib")

#elif defined(__APPLE__)

#include <sys/sysctl.h>
#include <CoreFoundation/CoreFoundation.h>
#include "utils_macos_interface.h"

#endif

using std::list;
using std::string;
using json = nlohmann::json;

namespace snowplow {
/**
 * @brief Tracker internal utility functions.
 */
class Utils {
public:
  static string get_uuid4();
  static string int_list_to_string(const list<int> &int_list, const string &delimiter);
  static string map_to_query_string(map<string, string> m);
  static string url_encode(string value);
  static string serialize_payload(Payload payload);
  static Payload deserialize_json_str(const string &json_str);
  static unsigned long long get_unix_epoch_ms();
  static SelfDescribingJson get_desktop_context();
  static string get_os_type();
  static string get_os_version();
  static string get_os_service_pack();
  static bool get_os_is_64bit();
  static string get_device_manufacturer();
  static string get_device_model();
  static int get_device_processor_count();

private:
  static SelfDescribingJson *m_desktop_context;
};
} // namespace snowplow

#endif
