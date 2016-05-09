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
#include <regex>
#include "http_request_result.hpp"

using namespace std;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#include <windows.h>
#include <WinInet.h>
#include <tchar.h>

#pragma comment (lib, "wininet.lib")

#elif defined(__APPLE__)

#include <iostream>
#include <fstream>
#include <sstream>
#include <CoreFoundation/CoreFoundation.h>
#include <CFNetwork/CFNetwork.h>
#include <CFNetwork/CFHTTPStream.h>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#endif

class HttpClient {
private:
  enum RequestMethod { POST, GET };
  static HttpRequestResult http_request(const RequestMethod method, const string & host, const string & path, 
    const string & post_data, bool use_default_port, unsigned int port, bool use_https, list<int> row_ids, bool oversize);

public:
  struct CrackedUrl {
    string hostname;
    string path;
    bool is_https;
    bool is_valid;
    int error_code;
    unsigned int port;
    bool use_default_port;
  };

  static const string TRACKER_AGENT;

  static CrackedUrl crack_url(const string & url);
  
  static HttpRequestResult http_post(const string & url, const string & post_data, list<int> row_ids, bool oversize);
  static HttpRequestResult http_get(const string & url, list<int> row_ids, bool oversize);
};

#endif
