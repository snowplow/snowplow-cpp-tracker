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

#ifndef HTTP_REQUEST_RESULT_H
#define HTTP_REQUEST_RESULT_H

#include <list>

using namespace std;

class HttpRequestResult {
private:
  int m_http_response_code;
  int m_internal_error_code;
  list<int> m_row_ids;
  bool m_is_successful;

public:
  HttpRequestResult();
  HttpRequestResult(int internal_error_code, int http_response_code, list<int> row_ids, bool oversize);
  int get_http_response_code();
  list<int> get_row_ids();
  bool is_success();
};

#endif
