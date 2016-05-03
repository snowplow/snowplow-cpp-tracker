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

#include "http_client.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

const string HttpClient::TRACKER_AGENT = string("Snowplow C++ Tracker (Win32)");

HttpClient::CrackedUrl HttpClient::crack_url(const string& url) {
  HttpClient::CrackedUrl cracked_url;

  string cleaned_url = url;
  if (regex_match(cleaned_url, regex("^https?://.+")) == false) {
    cleaned_url = string("http://") + url;
  }

  regex r_host("(https?)://([^\\s]+\\.[^\\s/]+)(/.*)?");
  regex r_hostname_port("([^:]+):(\\d+)");
  smatch match;

  if (regex_search(cleaned_url, match, r_host)) {
    string protocol = match.str(1);
    string hostname_port = match.str(2);
    cracked_url.path = match.str(3);

    smatch host_match;
    if (regex_search(hostname_port, host_match, r_hostname_port)) {
      cracked_url.hostname = host_match.str(1);
      string port = host_match.str(2);
      cracked_url.port = stoi(port);
      cracked_url.use_default_port = false;
    }
    else {
      cracked_url.hostname = hostname_port; // it's just a hostname
      cracked_url.port = 0;
      cracked_url.use_default_port = true;
    }

    cracked_url.is_https = protocol == "https";
    cracked_url.error_code = 0;
    cracked_url.is_valid = true;
  }
  else {
    cracked_url.error_code = -1;
    cracked_url.is_valid = false;
  }

  return cracked_url;
}

HttpRequestResult HttpClient::http_request(const RequestMethod method, const string & host, const string & path, const string & post_data, bool use_default_port, unsigned int port, bool use_https) {
  HINTERNET h_internet = InternetOpen(TEXT(HttpClient::TRACKER_AGENT.c_str()),
                                      INTERNET_OPEN_TYPE_DIRECT,
                                      NULL,
                                      NULL,
                                      0);

  if (h_internet == NULL) {
    return HttpRequestResult(GetLastError(), 0);
  }

  int use_port = port;
  if (use_default_port) {
    if (use_https) {
      use_port = INTERNET_DEFAULT_HTTPS_PORT;
    }
    else {
      use_port = INTERNET_DEFAULT_HTTP_PORT;
    }
  }

  HINTERNET h_connect = InternetConnect(h_internet,
                                        TEXT(host.c_str()),
                                        use_port,
                                        NULL,
                                        NULL,
                                        INTERNET_SERVICE_HTTP,
                                        0,
                                        NULL);

  if (h_connect == NULL) {
    InternetCloseHandle(h_internet);
    return HttpRequestResult(GetLastError(), 0);
  }

  DWORD flags = 0 | INTERNET_FLAG_RELOAD;
  if (use_https) {
    flags = flags | INTERNET_FLAG_SECURE;
  }

  string request_method_string;
  LPVOID post_buf;
  int post_buf_len;
  if (method == GET) {
    request_method_string = "GET";
    post_buf = NULL;
    post_buf_len = 0;
  }
  else {
    request_method_string = "POST";
    post_buf = (LPVOID)TEXT(post_data.c_str());
    post_buf_len = strlen(TEXT(post_data.c_str()));
  }

  HINTERNET h_request = HttpOpenRequest(h_connect,
                                        TEXT(request_method_string.c_str()),
                                        TEXT(path.c_str()),
                                        NULL,
                                        NULL,
                                        NULL,
                                        flags,
                                        0);

  if (h_request == NULL) {
    InternetCloseHandle(h_internet);
    InternetCloseHandle(h_connect);
    return HttpRequestResult(GetLastError(), 0);
  }

  BOOL is_sent = HttpSendRequest(h_request, NULL, 0, post_buf, post_buf_len);

  if (!is_sent) {
    return HttpRequestResult(GetLastError(), 0);
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
    NULL
  );

  InternetCloseHandle(h_request);
  InternetCloseHandle(h_connect);
  InternetCloseHandle(h_internet);

  return HttpRequestResult(0, http_status_code);
}

HttpRequestResult HttpClient::http_post(const string&url, const string& post_data) {
  HttpClient::CrackedUrl cracked_url = HttpClient::crack_url(url);
  if (cracked_url.is_valid) {
    return HttpClient::http_request(
      POST,
      cracked_url.hostname,
      cracked_url.path,
      post_data,
      cracked_url.use_default_port,
      cracked_url.port,
      cracked_url.is_https);
  }
  else {
    throw invalid_argument("Invalid URL '" + url + "'");
  }
}

HttpRequestResult HttpClient::http_get(const string& url) {
  HttpClient::CrackedUrl cracked_url = HttpClient::crack_url(url);
  if (cracked_url.is_valid) {
    return HttpClient::http_request(GET,
      cracked_url.hostname,
      cracked_url.path,
      "",
      cracked_url.use_default_port,
      cracked_url.port,
      cracked_url.is_https);
  }
  else {
    throw invalid_argument("Invalid URL '" + url + "'");
  }
}

#else

int HttpClient::http_post(string url) {
  return 200;
}

int HttpClient::http_get(string url) {
  return 200;
}

#endif
