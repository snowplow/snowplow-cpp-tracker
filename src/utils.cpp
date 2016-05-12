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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

string Utils::get_uuid4() {
  UUID uuid = { 0 };
  string uid;

  ::UuidCreate(&uuid);
  RPC_CSTR szUuid = NULL;
  if (::UuidToStringA(&uuid, &szUuid) == RPC_S_OK) {
    uid = (char*)szUuid;
    ::RpcStringFreeA(&szUuid);
  }
  else {
    throw runtime_error("couldn't generate UUID!");
  }

  return uid;
}

#elif defined(__APPLE__)

string Utils::get_uuid4() {
  CFUUIDRef cf_uuid_ref = CFUUIDCreate(kCFAllocatorDefault);
  CFStringRef cf_uuid_str_ref = CFUUIDCreateString(kCFAllocatorDefault, cf_uuid_ref);

  string uuid(CFStringGetCStringPtr(cf_uuid_str_ref, kCFStringEncodingUTF8));

  CFRelease(cf_uuid_ref);
  CFRelease(cf_uuid_str_ref);

  return uuid;
}

#endif

string Utils::int_list_to_string(list<int>* int_list, const string & delimiter) {
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
    escaped << '%' << uppercase << setw(2) << int((unsigned char) c);
    escaped << nouppercase;
  }

  return escaped.str();
}

string Utils::serialize_payload(Payload payload) {
  json j_map(payload.get());
  return j_map.dump();
}

Payload Utils::deserialize_json_str(const string & json_str) {
  Payload p;
  json j = json::parse(json_str);
  
  for (json::iterator it = j.begin(); it != j.end(); ++it) {
    p.add(it.key(), it.value());
  }

  return p;
}

unsigned long long Utils::get_unix_epoch_ms() {
  return chrono::duration_cast<chrono::milliseconds> (chrono::system_clock::now().time_since_epoch()).count();
}
