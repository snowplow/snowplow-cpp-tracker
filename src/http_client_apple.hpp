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

#ifndef HTTP_CLIENT_APPLE_H
#define HTTP_CLIENT_APPLE_H
#if defined(__APPLE__)

#include <map>
#include <string>
#include <regex>
#include "constants.hpp"
#include "cracked_url.hpp"
#include "http_request_result.hpp"
#include "ihttp_client.hpp"

#include <iostream>
#include <sstream>
#include <CoreFoundation/CoreFoundation.h>
#include <CFNetwork/CFNetwork.h>
#include <CFNetwork/CFHTTPStream.h>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

using std::string;
using std::list;
using std::mutex;

namespace snowplow {
/**
 * @brief HTTP client for making requests to Snowplow Collector. To be used internally within tracker only.
 */
class HttpClientApple : public IHttpClient {
public:
  ~HttpClientApple() {}

  static const string TRACKER_AGENT;
  HttpRequestResult http_post(const CrackedUrl url, const string & post_data, list<int> row_ids, bool oversize);
  HttpRequestResult http_get(const CrackedUrl url, const string & query_string, list<int> row_ids, bool oversize);

private:
  HttpRequestResult http_request(const RequestMethod method, const CrackedUrl url, const string & query_string, const string & post_data, list<int> row_ids, bool oversize);
};
}

#endif
#endif
