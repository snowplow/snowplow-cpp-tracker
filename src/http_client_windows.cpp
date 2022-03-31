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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include "http_client_windows.hpp"

using namespace snowplow;
using std::cerr;
using std::endl;
using std::lock_guard;

HttpRequestResult HttpClientWindows::http_post(const CrackedUrl url, const string &post_data, list<int> row_ids, bool oversize) {
  HttpRequestResult res = this->http_request(POST, url, "", post_data, row_ids, oversize);
  return res;
}

HttpRequestResult HttpClientWindows::http_get(const CrackedUrl url, const string &query_string, list<int> row_ids, bool oversize) {
  HttpRequestResult res = this->http_request(GET, url, query_string, "", row_ids, oversize);
  return res;
}

const string HttpClient::TRACKER_AGENT = string("Snowplow C++ Tracker (Win32)");

HttpRequestResult HttpClient::http_request(const RequestMethod method, CrackedUrl url, const string &query_string, const string &post_data, list<int> row_ids, bool oversize) {

  HINTERNET h_internet = InternetOpen(
      TEXT(HttpClient::TRACKER_AGENT.c_str()),
      INTERNET_OPEN_TYPE_DIRECT,
      NULL,
      NULL,
      0);

  if (h_internet == NULL) {
    return HttpRequestResult(GetLastError(), 0, row_ids, oversize);
  }

  unsigned int use_port = url.get_port();
  if (url.get_use_default_port()) {
    if (url.get_is_https()) {
      use_port = INTERNET_DEFAULT_HTTPS_PORT;
    } else {
      use_port = INTERNET_DEFAULT_HTTP_PORT;
    }
  }

  HINTERNET h_connect = InternetConnect(
      h_internet,
      TEXT(url.get_hostname().c_str()),
      use_port,
      NULL,
      NULL,
      INTERNET_SERVICE_HTTP,
      0,
      NULL);

  if (h_connect == NULL) {
    InternetCloseHandle(h_internet);
    return HttpRequestResult(GetLastError(), 0, row_ids, oversize);
  }

  DWORD flags = 0 | INTERNET_FLAG_RELOAD;
  if (url.get_is_https()) {
    flags = flags | INTERNET_FLAG_SECURE;
  }

  string final_path = url.get_path();
  string request_method_string;
  LPVOID post_buf;
  int post_buf_len;
  if (method == GET) {
    request_method_string = "GET";
    post_buf = NULL;
    post_buf_len = 0;
    final_path += "?" + query_string;
  } else {
    request_method_string = "POST";
    post_buf = (LPVOID)TEXT(post_data.c_str());
    post_buf_len = strlen(TEXT(post_data.c_str()));
  }

  HINTERNET h_request = HttpOpenRequest(
      h_connect,
      TEXT(request_method_string.c_str()),
      TEXT(final_path.c_str()),
      NULL,
      NULL,
      NULL,
      flags,
      0);

  if (h_request == NULL) {
    InternetCloseHandle(h_internet);
    InternetCloseHandle(h_connect);
    return HttpRequestResult(GetLastError(), 0, row_ids, oversize);
  }

  LPCSTR hdrs = TEXT("Content-Type: application/json; charset=utf-8");
  BOOL is_sent = HttpSendRequest(h_request, hdrs, strlen(hdrs), post_buf, post_buf_len);

  if (!is_sent) {
    InternetCloseHandle(h_internet);
    InternetCloseHandle(h_connect);
    InternetCloseHandle(h_request);
    return HttpRequestResult(GetLastError(), 0, row_ids, oversize);
  }

  string response;
  const int buf_len = 1024;
  char buff[buf_len];

  BOOL is_more = true;
  DWORD bytes_read = -1;

  while (is_more && bytes_read != 0) {
    is_more = InternetReadFile(h_request, buff, buf_len, &bytes_read);
    response.append(buff, bytes_read);
  }

  DWORD http_status_code = 0;
  DWORD length = sizeof(DWORD);
  HttpQueryInfo(
      h_request,
      HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
      &http_status_code,
      &length,
      NULL);

  InternetCloseHandle(h_request);
  InternetCloseHandle(h_connect);
  InternetCloseHandle(h_internet);

  return HttpRequestResult(0, http_status_code, row_ids, oversize);
}

#endif
