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

#ifndef HTTP_CLIENT_CURL_H
#define HTTP_CLIENT_CURL_H
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32) || defined(__CYGWIN__)

#include "http_client.hpp"

#include <string>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

using std::string;
using std::list;
using std::mutex;

namespace snowplow {
/**
 * @brief HTTP client that uses the Curl library for making requests to Snowplow Collector.
 * 
 * This HTTP client supports Unix systems with the curl library installed.
 */
class HttpClientCurl : public HttpClient {
public:
  HttpClientCurl();
  ~HttpClientCurl();

  static const string TRACKER_AGENT;

protected:
  HttpRequestResult http_request(const RequestMethod method, const CrackedUrl url, const string & query_string, const string & post_data, list<int> row_ids, bool oversize);
};
}

#endif
#endif
