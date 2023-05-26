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

#ifndef HTTP_CLIENT_WINDOWS_H
#define HTTP_CLIENT_WINDOWS_H
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#include <string>
#include "http_client.hpp"

#include <windows.h>
#include <WinInet.h>
#include <tchar.h>

#pragma comment (lib, "wininet.lib")

namespace snowplow {

using std::string;
using std::list;

/**
 * @brief HTTP client for making requests to Snowplow Collector using Windows APIs.
 * 
 * This HTTP client is only compatible with Windows.
 */
class HttpClientWindows : public HttpClient {
public:
  static const string TRACKER_AGENT;

protected:
  HttpRequestResult http_request(const RequestMethod method, const CrackedUrl url, const string & query_string, const string & post_data, list<int> row_ids, bool oversize);
};
}

#endif
#endif
