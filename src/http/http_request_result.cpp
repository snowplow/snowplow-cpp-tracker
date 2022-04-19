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

#include "http_request_result.hpp"
#include "../constants.hpp"

using namespace snowplow;

HttpRequestResult::HttpRequestResult() {
  m_is_oversize = false;
  m_internal_error_code = 0; // not an error
  m_http_response_code = 0; // not success, should retry
  m_row_ids = {};
}

HttpRequestResult::HttpRequestResult(int internal_error_code, int http_response_code, list<int> row_ids, bool oversize) {
  m_is_oversize = oversize;
  m_internal_error_code = internal_error_code;
  m_http_response_code = internal_error_code != 0 ? -1 : http_response_code;
  m_row_ids = row_ids;
}

int HttpRequestResult::get_http_response_code() const {
  return m_http_response_code;
}

list<int> HttpRequestResult::get_row_ids() const {
  return m_row_ids;
}

bool HttpRequestResult::is_internal_error() const {
  return m_internal_error_code != 0;
}

bool HttpRequestResult::is_success() const {
  if (is_internal_error()) {
    return false;
  }
  return (get_http_response_code() >= 200 && get_http_response_code() < 300);
}

bool HttpRequestResult::should_retry() const {
  // don't retry if successful
  if (is_success()) {
    return false;
  }

  // don't retry if request is larger than max byte limit
  if (m_is_oversize) {
    return false;
  }

  return true;
}
