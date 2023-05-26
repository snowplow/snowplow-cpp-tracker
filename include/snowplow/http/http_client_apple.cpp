/*
Copyright (c) 2023 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#if defined(__APPLE__)
#include "http_client_apple.hpp"
#include "../detail/http/request_macos_interface.h"

using namespace snowplow;

HttpRequestResult HttpClientApple::http_request(const RequestMethod method, CrackedUrl url, const string &query_string, const string &post_data, list<int> row_ids, bool oversize) {
  string final_url = url.to_string();
  if (method == GET) {
    final_url += "?" + query_string;
  }

  int status_code = make_request(
    method == POST,
    final_url,
    post_data
  );

  return HttpRequestResult(0, status_code, row_ids, oversize);
}

#endif
