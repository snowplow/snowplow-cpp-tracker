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

#ifndef MICRO_H
#define MICRO_H

#include <string>
#include <list>
#include "../../include/snowplow/thirdparty/json.hpp"

using std::string;
using std::tuple;
using std::list;
using nlohmann::json;

namespace snowplow {
const string SNOWPLOW_MICRO_HOSTNAME = "127.0.0.1";
const string SNOWPLOW_MICRO_DOMAIN = SNOWPLOW_MICRO_HOSTNAME + ":9090";
const string SNOWPLOW_MICRO_ENDPOINT = "http://" + SNOWPLOW_MICRO_DOMAIN;

class Micro {
public:
    static void clear();
    static tuple<int, int> get_good_and_bad_count();
    static list<json> get_good();

private:
    static string request(const string &path);
};
}

#endif
