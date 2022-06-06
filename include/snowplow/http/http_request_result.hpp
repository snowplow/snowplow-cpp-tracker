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

#ifndef HTTP_REQUEST_RESULT_H
#define HTTP_REQUEST_RESULT_H

#include <iostream>
#include <list>
#include <map>

using std::list;
using std::map;

namespace snowplow {
/**
 * @brief Response from HTTP requests to collector. To be used internally within tracker only.
 */
class HttpRequestResult {
public:
  HttpRequestResult();
  HttpRequestResult(int internal_error_code, int http_response_code, list<int> row_ids, bool oversize);
  int get_http_response_code() const;
  list<int> get_row_ids() const;
  bool is_success() const;
  bool should_retry(const map<int, bool> &custom_retry_for_status_codes) const;

private:
  bool is_internal_error() const;

  int m_http_response_code;
  int m_internal_error_code;
  list<int> m_row_ids;
  bool m_is_oversize;
};
} // namespace snowplow

#endif
