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

#include "micro.hpp"

using namespace snowplow;

void Micro::clear() {
  request("/micro/reset");
}

tuple<int, int> Micro::get_good_and_bad_count() {
  string response_str = request("/micro/all");
  json response = json::parse(response_str);
  int good = response["good"].get<int>();
  int bad = response["bad"].get<int>();
  return std::make_tuple(good, bad);
}

list<json> Micro::get_good() {
  string response_str = request("/micro/good");
  return json::parse(response_str).get<list<json>>();
}

static size_t write_data(void *data, size_t byte_size, size_t n_bytes, string *body) {
  ((std::string*)body)->append((char*)data, byte_size * n_bytes);
  return byte_size * n_bytes;
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#include <WinInet.h>
#include <tchar.h>

string Micro::request(const string &path) {
  HINTERNET h_internet = InternetOpen(
      TEXT("Test"),
      INTERNET_OPEN_TYPE_DIRECT,
      NULL,
      NULL,
      0);

  HINTERNET h_connect = InternetConnectA(
      h_internet,
      SNOWPLOW_MICRO_HOSTNAME.c_str(),
      9090,
      NULL,
      NULL,
      INTERNET_SERVICE_HTTP,
      0,
      NULL);

  HINTERNET h_request = HttpOpenRequestA(
      h_connect,
      "GET",
      path.c_str(),
      NULL,
      NULL,
      NULL,
      0 | INTERNET_FLAG_RELOAD,
      0);

  LPCTSTR hdrs = TEXT("Content-Type: application/json; charset=utf-8");
  BOOL is_sent = HttpSendRequest(h_request, hdrs, strlen(hdrs), NULL, 0);

  string response;
  const int buf_len = 1024;
  char buff[buf_len];

  BOOL is_more = true;
  DWORD bytes_read = -1;

  while (is_more && bytes_read != 0) {
    is_more = InternetReadFile(h_request, buff, buf_len, &bytes_read);
    response.append(buff, bytes_read);
  }

  InternetCloseHandle(h_request);
  InternetCloseHandle(h_connect);
  InternetCloseHandle(h_internet);

  return response;
}

#else

#include <curl/curl.h>

string Micro::request(const string &path) {
  CURL *curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

  string read_buffer;
  string full_url = SNOWPLOW_MICRO_ENDPOINT + path;
  curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

  // send the request
  CURLcode res = curl_easy_perform(curl);
  long status_code;
  if (res == CURLE_OK) {
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
  }
  
  // cleanup
  curl_easy_cleanup(curl);

  return read_buffer;
}
#endif
