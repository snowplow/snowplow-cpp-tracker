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

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <map>
#include <string>
#include "http_request_result.hpp"

using namespace std;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#include <windows.h>
#include <WinInet.h>
#include <tchar.h>
#include <regex>

#pragma comment (lib, "wininet.lib")

class HttpClient {
private:
  static HttpRequestResult http_get(const string & host, const string & path, unsigned int port, bool use_https);

public:
  struct CrackedUrl {
    string hostname;
    string path;
    bool is_https;
    bool is_valid;
    int error_code;
    unsigned int port;
  };

  static const string TRACKER_AGENT;

  static CrackedUrl crackUrl(const string&);
  static int http_post(string);
  static HttpRequestResult http_get(const string&);
};

#endif

#endif
