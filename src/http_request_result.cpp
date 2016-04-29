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

HttpRequestResult::HttpRequestResult(int error_code, int http_return_code) {
  if (error_code != 0) {
    this->http_response_code = -1;
    this->internal_error_code = error_code;
    this->is_successful = false;
  } else {
    this->http_response_code = http_return_code;

    if (http_response_code == 200) {
      this->is_successful = true;
    } else {
      this->is_successful = false;
    }

    this->internal_error_code = 0;
  }
}

int HttpRequestResult::get_http_response_code() {
  return this->http_response_code;
}

bool HttpRequestResult::is_success() {
  return this->is_successful;
}
