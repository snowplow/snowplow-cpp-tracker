/*
Copyright (c) 2016 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#include "http_request_result.hpp"

HttpRequestResult::HttpRequestResult() {}

HttpRequestResult::HttpRequestResult(int internal_error_code, int http_response_code, list<int> row_ids, bool oversize) {
  if (oversize) {
    this->m_internal_error_code = 0;
    this->m_http_response_code = 200;
    this->m_is_successful = true;
  } else if (internal_error_code != 0) {
    this->m_internal_error_code = internal_error_code;
    this->m_http_response_code = -1;
    this->m_is_successful = false;
  } else {
    this->m_internal_error_code = 0;
    this->m_http_response_code = http_response_code;
    this->m_is_successful = (this->m_http_response_code == 200);
  }

  this->m_row_ids = row_ids;
}

int HttpRequestResult::get_http_response_code() {
  return this->m_http_response_code;
}

list<int> HttpRequestResult::get_row_ids() {
  return this->m_row_ids;
}

bool HttpRequestResult::is_success() {
  return this->m_is_successful;
}
