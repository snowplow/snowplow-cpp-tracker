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

#include "test_http_client.hpp"

using namespace snowplow;
using std::cerr;
using std::endl;
using std::lock_guard;

const string TestHttpClient::TRACKER_AGENT = string("Snowplow C++ Tracker (Integration tests)");

list<TestHttpClient::Request> TestHttpClient::requests_list;
mutex TestHttpClient::log_read_write;
int TestHttpClient::response_code = 200;

HttpRequestResult TestHttpClient::http_request(const RequestMethod method, CrackedUrl url, const string &query_string, const string &post_data, list<int> row_ids, bool oversize) {
  lock_guard<mutex> guard(log_read_write);

  TestHttpClient::Request r;
  r.method = method;
  r.query_string = query_string;
  r.post_data = post_data;
  r.row_ids = row_ids;
  r.oversize = oversize;
  requests_list.push_back(r);

  return HttpRequestResult(0, response_code, row_ids, oversize);
}

void TestHttpClient::set_http_response_code(int http_response_code) {
  lock_guard<mutex> guard(log_read_write);
  response_code = http_response_code;
}

list<TestHttpClient::Request> TestHttpClient::get_requests_list() {
  lock_guard<mutex> guard(log_read_write);
  return requests_list;
}

void TestHttpClient::reset() {
  lock_guard<mutex> guard(log_read_write);
  requests_list.clear();
  response_code = 200;
}
