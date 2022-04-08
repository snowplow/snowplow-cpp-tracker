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

#include "../src/utils.hpp"
#include "catch.hpp"
#include <algorithm>
#include <regex>

using namespace snowplow;
using std::regex;
using std::to_string;

TEST_CASE("utils") {
  SECTION("get_uuid4 should return valid uuid up to the spefication") {
    regex r_uuid4("[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-4[0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}");
    REQUIRE(true == regex_match(Utils::get_uuid4(), r_uuid4));
  }

  SECTION("get_uuid4 should return different values on each run") {
    string first = Utils::get_uuid4();
    string second = Utils::get_uuid4();
    REQUIRE(first != second);
  }

  SECTION("get_uuid4 should return lowercase values") {
    string uuid = Utils::get_uuid4();
    string uuid_lower = uuid;
    std::transform(uuid_lower.begin(), uuid_lower.end(), uuid_lower.begin(), ::tolower);
    REQUIRE(uuid_lower == uuid);
  }

  SECTION("int_list_to_string will successfully convert a list of integers to a string") {
    list<int> *int_list = new list<int>;
    int_list->push_back(1);
    int_list->push_back(2);
    int_list->push_back(3);
    int_list->push_back(4);
    int_list->push_back(5);

    REQUIRE("1,2,3,4,5" == Utils::int_list_to_string(int_list, ","));

    int_list->clear();
    delete (int_list);
  }

  SECTION("map_to_query_string should correctly convert a map<string,string> to a query string") {
    map<string, string> queryPairs;
    queryPairs["e"] = "pv";
    queryPairs["k2"] = "s p a c e";
    queryPairs["k3"] = "s+p+a+c+e";

    REQUIRE("e=pv&k2=s%20p%20a%20c%20e&k3=s%2Bp%2Ba%2Bc%2Be" == Utils::map_to_query_string(queryPairs));
  }

  SECTION("url_encode should correctly encode a string for sending as part of a url") {
    REQUIRE("e%20pv" == Utils::url_encode("e pv"));
    REQUIRE("%3C%20%3E%20%23%20%25%20%7B%20%7D%20%7C%20%5C%20%5E%20%7E%20%5B%20%5D%20%60%20%3B%20%2F%20%3F%20%3A%20%40%20%3D%20%26%20%24%20%2B%20%22" ==
            Utils::url_encode("< > # % { } | \\ ^ ~ [ ] ` ; / ? : @ = & $ + \""));
  }

  SECTION("serialize_payload will successfully convert a Payload into a JSON string") {
    Payload p;
    p.add("e", "pv");
    p.add("p", "srv");
    p.add("tv", "cpp-0.1.0");

    REQUIRE("{\"e\":\"pv\",\"p\":\"srv\",\"tv\":\"cpp-0.1.0\"}" == Utils::serialize_payload(p));
  }

  SECTION("deserialize_json_str will successfully convert a JSON string into a Payload") {
    string j_str = "{\"e\":\"pv\",\"p\":\"srv\",\"tv\":\"cpp-0.1.0\"}";
    Payload p = Utils::deserialize_json_str(j_str);

    REQUIRE(p.get()["e"] == "pv");
    REQUIRE(p.get()["p"] == "srv");
    REQUIRE(p.get()["tv"] == "cpp-0.1.0");
  }

  SECTION("get_unix_epoch_ms should return the time since epoch in milliseconds") {
    REQUIRE(13 == std::to_string(Utils::get_unix_epoch_ms()).length());
  }

  SECTION("get_device_context should populate OS specific information correctly") {
    string os_type = Utils::get_os_type();
    string os_version = Utils::get_os_version();
    string os_service_pack = Utils::get_os_service_pack();
    bool os_is_64bit = Utils::get_os_is_64bit();
    string device_manufacturer = Utils::get_device_manufacturer();
    string device_model = Utils::get_device_model();
    int device_processor_count = Utils::get_device_processor_count();

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    DWORD expected_proc_count = std::thread::hardware_concurrency();

    OSVERSIONINFOEX osviex;
    ::ZeroMemory(&osviex, sizeof(OSVERSIONINFOEX));
    osviex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    REQUIRE(::GetVersionEx((LPOSVERSIONINFO)&osviex) != 0);

    string expected_os_version = to_string(osviex.dwMajorVersion) + "." + to_string(osviex.dwMinorVersion) + "." + to_string(osviex.dwBuildNumber);
    string expected_service_pack = to_string(osviex.wServicePackMajor) + "." + to_string(osviex.wServicePackMinor);

    bool expected_is_64_bit_os;

#if defined(_WIN64)
    expected_is_64_bit_os = true;
#elif defined(_WIN32)
    BOOL f64 = FALSE;
    expected_is_64_bit_os = IsWow64Process(GetCurrentProcess(), &f64) && f64;
#else
    expected_is_64_bit_os = false;
#endif
    REQUIRE("Windows" == os_type);
    REQUIRE(expected_os_version == os_version);
    REQUIRE(expected_service_pack == os_service_pack);
    REQUIRE(expected_is_64_bit_os == os_is_64bit);
    REQUIRE("" == device_manufacturer);
    REQUIRE("" == device_model);
    REQUIRE(expected_proc_count == device_processor_count);
#elif defined(__APPLE__)
    REQUIRE("macOS" == os_type);
    REQUIRE("" != os_version);
    REQUIRE("" == os_service_pack);
    REQUIRE("Apple Inc." == device_manufacturer);
    REQUIRE("" != device_model);
    REQUIRE(0 != device_processor_count);
#else
    REQUIRE("Linux" == os_type);
    REQUIRE("" != os_version);
    REQUIRE("" == os_service_pack);
    REQUIRE("" == device_manufacturer);
    REQUIRE("" == device_model);
    REQUIRE(0 != device_processor_count);
#endif

    SelfDescribingJson desktop_context = Utils::get_desktop_context();
    json desktop_context_json = desktop_context.get();
    json desktop_context_data = desktop_context_json[SNOWPLOW_DATA];

    REQUIRE(SNOWPLOW_SCHEMA_DESKTOP_CONTEXT == desktop_context_json[SNOWPLOW_SCHEMA].get<std::string>());
    REQUIRE(os_type == desktop_context_data[SNOWPLOW_DESKTOP_OS_TYPE].get<std::string>());
    REQUIRE(os_version == desktop_context_data[SNOWPLOW_DESKTOP_OS_VERSION].get<std::string>());
    REQUIRE(os_service_pack == desktop_context_data[SNOWPLOW_DESKTOP_OS_SERVICE_PACK].get<std::string>());
    REQUIRE(os_is_64bit == desktop_context_data[SNOWPLOW_DESKTOP_OS_IS_64_BIT].get<bool>());
    REQUIRE(device_manufacturer == desktop_context_data[SNOWPLOW_DESKTOP_DEVICE_MANU].get<std::string>());
    REQUIRE(device_model == desktop_context_data[SNOWPLOW_DESKTOP_DEVICE_MODEL].get<std::string>());
    REQUIRE(device_processor_count == desktop_context_data[SNOWPLOW_DESKTOP_DEVICE_PROC_COUNT].get<int>());
  }
}
