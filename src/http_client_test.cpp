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

#include "http_client_test.hpp"

using namespace snowplow;
using std::cerr;
using std::endl;
using std::lock_guard;

HttpRequestResult HttpClientTest::http_post(const CrackedUrl url, const string &post_data, list<int> row_ids, bool oversize) {
  HttpRequestResult res = this->http_request(POST, url, "", post_data, row_ids, oversize);
  return res;
}

HttpRequestResult HttpClientTest::http_get(const CrackedUrl url, const string &query_string, list<int> row_ids, bool oversize) {
  HttpRequestResult res = this->http_request(GET, url, query_string, "", row_ids, oversize);
  return res;
}

const string HttpClientTest::TRACKER_AGENT = string("Snowplow C++ Tracker (Integration tests)");

list<HttpClientTest::Request> HttpClientTest::requests_list;
mutex HttpClientTest::log_read_write;
int HttpClientTest::response_code = 200;

HttpRequestResult HttpClientTest::http_request(const RequestMethod method, CrackedUrl url, const string &query_string, const string &post_data, list<int> row_ids, bool oversize) {
  lock_guard<mutex> guard(log_read_write);

  HttpClientTest::Request r;
  r.method = method;
  r.query_string = query_string;
  r.post_data = post_data;
  r.row_ids = row_ids;
  r.oversize = oversize;
  requests_list.push_back(r);

  return HttpRequestResult(0, response_code, row_ids, oversize);
}

void HttpClientTest::set_http_response_code(int http_response_code) {
  lock_guard<mutex> guard(log_read_write);
  response_code = http_response_code;
}

list<HttpClientTest::Request> HttpClientTest::get_requests_list() {
  lock_guard<mutex> guard(log_read_write);
  return requests_list;
}

void HttpClientTest::reset() {
  lock_guard<mutex> guard(log_read_write);
  requests_list.clear();
  response_code = 200;
}
