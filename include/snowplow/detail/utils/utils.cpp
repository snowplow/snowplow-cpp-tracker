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

#include "utils.hpp"

using namespace snowplow;
using std::hex;
using std::nouppercase;
using std::ostringstream;
using std::runtime_error;
using std::setw;
using std::stringstream;
using std::to_string;
using std::uppercase;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

string Utils::get_uuid4() {
  UUID uuid = {0};
  string uid;

  ::UuidCreate(&uuid);
  RPC_CSTR szUuid = NULL;
  if (::UuidToStringA(&uuid, &szUuid) == RPC_S_OK) {
    uid = (char *)szUuid;
    ::RpcStringFreeA(&szUuid);
  } else {
    throw runtime_error("FATAL: Could not generate unique UUID");
  }

  return uid;
}

#elif defined(__APPLE__)

string Utils::get_uuid4() {
  CFUUIDRef cf_uuid_ref = CFUUIDCreate(kCFAllocatorDefault);
  CFStringRef cf_uuid_str_ref = CFUUIDCreateString(kCFAllocatorDefault, cf_uuid_ref);

  string uuid(CFStringGetCStringPtr(cf_uuid_str_ref, kCFStringEncodingUTF8));
  transform(uuid.begin(), uuid.end(), uuid.begin(), ::tolower);

  CFRelease(cf_uuid_ref);
  CFRelease(cf_uuid_str_ref);

  return uuid;
}

#else

#include <uuid/uuid.h>

string Utils::get_uuid4() {
  uuid_t uuid;
  char str[200];
  uuid_generate_random(uuid);
  uuid_unparse(uuid, str);
  return string(str);
}

#endif

string Utils::int_list_to_string(const list<int> &int_list, const string &delimiter) {
  stringstream s;
  int i = 0;
  int length = int_list.size();

  for (auto const &value : int_list) {
    s << value;
    if (i < length - 1) {
      s << delimiter;
    }
    i++;
  }

  return s.str();
}

string Utils::map_to_query_string(map<string, string> m) {
  stringstream s;
  int i;
  map<string, string>::iterator it;

  int length = m.size();
  for (i = 0, it = m.begin(); it != m.end(); ++it, ++i) {
    s << Utils::url_encode(it->first) << "=" << Utils::url_encode(it->second);
    if (i < length - 1) {
      s << "&";
    }
  }

  return s.str();
}

string Utils::url_encode(string value) {
  ostringstream escaped;
  string::iterator i;

  escaped.fill('0');
  escaped << hex;

  for (i = value.begin(); i != value.end(); ++i) {
    char c = (*i);
    if (isalnum(c) || c == '-' || c == '_' || c == '.') {
      escaped << c;
      continue;
    }
    escaped << uppercase;
    escaped << '%' << uppercase << setw(2) << int((unsigned char)c);
    escaped << nouppercase;
  }

  return escaped.str();
}

string Utils::serialize_payload(Payload payload) {
  json j_map(payload.get());
  return j_map.dump();
}

Payload Utils::deserialize_json_str(const string &json_str) {
  Payload p;
  json j = json::parse(json_str);

  for (json::iterator it = j.begin(); it != j.end(); ++it) {
    p.add(it.key(), it.value());
  }

  return p;
}

unsigned long long Utils::get_unix_epoch_ms() {
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

// --- Desktop Context

SelfDescribingJson *Utils::m_desktop_context = 0;

SelfDescribingJson Utils::get_desktop_context() {
  if (!m_desktop_context) {
    json data;
    data[SNOWPLOW_DESKTOP_OS_TYPE] = get_os_type();
    data[SNOWPLOW_DESKTOP_OS_VERSION] = get_os_version();
    data[SNOWPLOW_DESKTOP_OS_SERVICE_PACK] = get_os_service_pack();
    data[SNOWPLOW_DESKTOP_OS_IS_64_BIT] = get_os_is_64bit();
    data[SNOWPLOW_DESKTOP_DEVICE_MANU] = get_device_manufacturer();
    data[SNOWPLOW_DESKTOP_DEVICE_MODEL] = get_device_model();
    data[SNOWPLOW_DESKTOP_DEVICE_PROC_COUNT] = get_device_processor_count();
    m_desktop_context = new SelfDescribingJson(SNOWPLOW_SCHEMA_DESKTOP_CONTEXT, data);
  }

  SelfDescribingJson copy = *m_desktop_context;
  return copy;
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

string Utils::get_os_type() {
  return "Windows";
}

string Utils::get_os_version() {
  OSVERSIONINFOEX osviex;
  ::ZeroMemory(&osviex, sizeof(OSVERSIONINFOEX));
  osviex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  ::GetVersionEx((LPOSVERSIONINFO)&osviex);
  return to_string(osviex.dwMajorVersion) + "." + to_string(osviex.dwMinorVersion) + "." + to_string(osviex.dwBuildNumber);
}

string Utils::get_os_service_pack() {
  OSVERSIONINFOEX osviex;
  ::ZeroMemory(&osviex, sizeof(OSVERSIONINFOEX));
  osviex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  ::GetVersionEx((LPOSVERSIONINFO)&osviex);
  return to_string(osviex.wServicePackMajor) + "." + to_string(osviex.wServicePackMinor);
}

string Utils::get_device_manufacturer() {
  return "";
}

string Utils::get_device_model() {
  return "";
}

bool Utils::get_os_is_64bit() {
#if defined(_WIN64)
  return true;
#elif defined(_WIN32)
  BOOL f64 = FALSE;
  return IsWow64Process(GetCurrentProcess(), &f64) && f64;
#else
  return false;
#endif
}

int Utils::get_device_processor_count() {
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);

  return sysinfo.dwNumberOfProcessors;
}

#else

bool Utils::get_os_is_64bit() {
#if INTPTR_MAX == INT64_MAX
  return true;
#else
  return false;
#endif
}

int Utils::get_device_processor_count() {
  return std::thread::hardware_concurrency();
}

string Utils::get_os_service_pack() {
  return "";
}

#if defined(__APPLE__)

string Utils::get_os_type() {
  return "macOS";
}

string Utils::get_os_version() {
  return get_os_version_objc();
}

string Utils::get_device_manufacturer() {
  return "Apple Inc.";
}

string Utils::get_device_model() {
  char str[256];
  size_t size = sizeof(str);
  int ret = sysctlbyname("hw.model", str, &size, NULL, 0);
  return str;
}

#else

#include <sys/utsname.h>

string Utils::get_os_type() {
  utsname info;
  uname(&info);
  return info.sysname; // e.g., Linux
}

string Utils::get_os_version() {
  utsname info;
  uname(&info);
  return info.version; // e.g., #26~20.04.1-Ubuntu SMP Sat Jan 8 18:05:46 UTC 2022
}

string Utils::get_device_manufacturer() {
  return "";
}

string Utils::get_device_model() {
  return "";
}

#endif
#endif

string Utils::get_unix_epoch_ms_as_datetime_string(unsigned long long timestamp_ms) {
  std::time_t time = timestamp_ms / 1000;
  std::tm* t = std::gmtime(&time);
  int ms = timestamp_ms % 1000;

  stringstream ss;
  ss << std::put_time(t, "%Y-%m-%dT%H:%M:%S")
    << '.' << std::setfill('0') << std::setw(3) << ms << "Z";
  return ss.str();
}
