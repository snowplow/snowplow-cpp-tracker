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
int TestHttpClient::temporary_response_code = -1;
int TestHttpClient::temporary_response_code_remaining_attempts = 0;

HttpRequestResult TestHttpClient::http_request(const RequestMethod method, CrackedUrl url, const string &query_string, const string &post_data, list<int> row_ids, bool oversize) {
  lock_guard<mutex> guard(log_read_write);

  TestHttpClient::Request r;
  r.method = method;
  r.query_string = query_string;
  r.post_data = post_data;
  r.row_ids = row_ids;
  r.oversize = oversize;
  requests_list.push_back(r);
  m_requests_list.push_back(r);

  return HttpRequestResult(0, fetch_response_code(), row_ids, oversize);
}

void TestHttpClient::set_http_response_code(int http_response_code) {
  lock_guard<mutex> guard(log_read_write);
  response_code = http_response_code;
}

void TestHttpClient::set_temporary_response_code(int http_response_code, int number_of_attempts) {
  lock_guard<mutex> guard(log_read_write);
  temporary_response_code = http_response_code;
  temporary_response_code_remaining_attempts = number_of_attempts;
}

int TestHttpClient::fetch_response_code() {
  if (temporary_response_code_remaining_attempts > 0) {
    int code = temporary_response_code;
    temporary_response_code_remaining_attempts--;
    return code;
  }
  return response_code;
}

list<TestHttpClient::Request> TestHttpClient::get_requests_list() {
  lock_guard<mutex> guard(log_read_write);
  return requests_list;
}

list<TestHttpClient::Request> TestHttpClient::get_instance_requests_list() {
  lock_guard<mutex> guard(log_read_write);
  return m_requests_list;
}

void TestHttpClient::reset() {
  lock_guard<mutex> guard(log_read_write);
  requests_list.clear();
  response_code = 200;
  temporary_response_code_remaining_attempts = 0;
}
