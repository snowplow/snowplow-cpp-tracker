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

#ifndef HTTP_CLIENT_TEST_H
#define HTTP_CLIENT_TEST_H

#include "../../src/http/http_client.hpp"

#include <mutex>

using std::string;
using std::list;
using std::mutex;

namespace snowplow {
/**
 * @brief HTTP client for making requests to Snowplow Collector. To be used internally within tracker only.
 */
class TestHttpClient : public HttpClient {
public:
  ~TestHttpClient() {}

  static const string TRACKER_AGENT;

  struct Request {
    Request(){};
    RequestMethod method;
    string query_string;
    string post_data;
    list<int> row_ids;
    bool oversize;
  };

  static list<Request> requests_list;
  static int response_code;
  static int temporary_response_code;
  static mutex log_read_write;

  static void set_http_response_code(int http_response_code);
  static void set_temporary_response_code(int http_response_code);
  static list<Request> get_requests_list();
  static void reset();

protected:
  HttpRequestResult http_request(const RequestMethod method, const CrackedUrl url, const string & query_string, const string & post_data, list<int> row_ids, bool oversize);
  
private:
  static int fetch_response_code();
};
}

#endif
