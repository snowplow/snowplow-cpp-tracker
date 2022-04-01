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

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include "cracked_url.hpp"
#include "http_request_result.hpp"

using std::string;
using std::list;

namespace snowplow {
/**
 * @brief Abstract base class for HTTP client for making requests to Snowplow Collector. It is used by Emitter.
 */
class HttpClient {
public:
  enum RequestMethod { POST, GET };

  virtual ~HttpClient() {}

  HttpRequestResult http_post(const CrackedUrl url, const string &post_data, list<int> row_ids, bool oversize) {
    return http_request(POST, url, "", post_data, row_ids, oversize);
  }
  HttpRequestResult http_get(const CrackedUrl url, const string &query_string, list<int> row_ids, bool oversize) {
    return http_request(GET, url, query_string, "", row_ids, oversize);
  }

protected:
  virtual HttpRequestResult http_request(const RequestMethod method, const CrackedUrl url, const string & query_string, const string & post_data, list<int> row_ids, bool oversize) = 0;
};
}

#endif
