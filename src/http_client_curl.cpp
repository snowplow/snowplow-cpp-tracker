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

#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32) || defined(__CYGWIN__)
#include "http_client_curl.hpp"
#include "constants.hpp"
#include "curl/curl.h"

using namespace snowplow;
using std::cerr;
using std::endl;
using std::lock_guard;

HttpClientCurl::HttpClientCurl() {
  curl_global_init(CURL_GLOBAL_ALL);
}

HttpClientCurl::~HttpClientCurl() {
  curl_global_cleanup();
}

const string HttpClientCurl::TRACKER_AGENT = string("Snowplow C++ Tracker (Unix)");

static size_t write_data(void *data, size_t byte_size, size_t n_bytes, std::string *body) {
  return byte_size * n_bytes;
}

HttpRequestResult HttpClientCurl::http_request(const RequestMethod method, CrackedUrl url, const string &query_string, const string &post_data, list<int> row_ids, bool oversize) {
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();

  if (curl) {
    // create the request
    std::ostringstream full_url_stream;
    full_url_stream << url.to_string();

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, ("User-Agent: " + TRACKER_AGENT).c_str());
    headers = curl_slist_append(headers, "Connection: keep-alive");

    if (method == GET) {
      full_url_stream << '?' << query_string;
      curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    } else {
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
      headers = curl_slist_append(headers, ("Content-Type: " + SNOWPLOW_POST_CONTENT_TYPE).c_str());
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    std::string full_url = full_url_stream.str();
    curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

    // send the request
    res = curl_easy_perform(curl);
    long status_code;
    if (res == CURLE_OK) {
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    }
    
    // cleanup
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    return HttpRequestResult(0, status_code, row_ids, oversize);
  }

  return HttpRequestResult(1, -1, row_ids, oversize);
}

#endif
