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

#ifndef IHTTP_CLIENT_H
#define IHTTP_CLIENT_H

#include <map>
#include <string>
#include <regex>
#include "constants.hpp"
#include "cracked_url.hpp"
#include "http_request_result.hpp"

using std::string;
using std::list;
using std::mutex;

namespace snowplow {
/**
 * @brief HTTP client for making requests to Snowplow Collector. To be used internally within tracker only.
 */
class IHttpClient {
public:
  enum RequestMethod { POST, GET };

  virtual ~IHttpClient() {}
  
  virtual HttpRequestResult http_post(const CrackedUrl url, const string & post_data, list<int> row_ids, bool oversize) = 0;
  virtual HttpRequestResult http_get(const CrackedUrl url, const string & query_string, list<int> row_ids, bool oversize) = 0;
};
}

#endif
