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

using namespace std;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

const string HttpClient::TRACKER_AGENT = string("Snowplow C++ Tracker (Win32)");

int HttpClient::http_post(string url) {
	return 200; // all ok
}

HttpClient::CrackedUrl HttpClient::crackUrl(const string& url) {
	HttpClient::CrackedUrl crackedUrl;

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
		crackedUrl.path = match.str(3);

		smatch host_match;
		if (regex_search(hostname_port, host_match, r_hostname_port)) {
			crackedUrl.hostname = host_match.str(1);
			string port = host_match.str(2);
			crackedUrl.port = atoi(port.c_str());
		}
		else {
			crackedUrl.hostname = hostname_port; // it's just a hostname
			crackedUrl.port = -1; // todo use option if available
		}

		crackedUrl.is_https = protocol == "https";
		crackedUrl.error_code = 0;
		crackedUrl.is_valid = true;
	}
	else {
		crackedUrl.error_code = -1;
		crackedUrl.is_valid = false;
	}

	return crackedUrl;
}

HttpRequestResult HttpClient::http_get(const string& url) {
	HttpClient::CrackedUrl crackedUrl = HttpClient::crackUrl(url);
	if (crackedUrl.is_valid) {
		return HttpClient::http_get(crackedUrl.hostname, crackedUrl.path, crackedUrl.port, crackedUrl.is_https);
	}
	else {
		throw invalid_argument("Invalid URL '" + url + "'");
	}
}

HttpRequestResult HttpClient::http_get(const string& host, const string& path, unsigned int port, bool use_https) {

	HINTERNET hInternet = InternetOpen(TEXT(HttpClient::TRACKER_AGENT.c_str()), 
		     		                   INTERNET_OPEN_TYPE_DIRECT, 
									   NULL,
									   NULL,
									   0);

	if (hInternet == NULL) {
		return HttpRequestResult(GetLastError(), 0);
	}

	HINTERNET hConnect = InternetConnect(hInternet, 
										TEXT(host.c_str()),
										80,
										NULL,
										NULL,
										INTERNET_SERVICE_HTTP,
										0,
										NULL);

	if (hConnect == NULL) {
		InternetCloseHandle(hInternet);
		return HttpRequestResult(GetLastError(), 0);
	}

	HINTERNET hRequest = HttpOpenRequest(hConnect,
										TEXT("GET"), 
										TEXT(path.c_str()),
										NULL,
										NULL,
										NULL,
										0 | INTERNET_FLAG_RELOAD,
										0);

	if (hRequest == NULL) {
		InternetCloseHandle(hInternet);
		InternetCloseHandle(hConnect);
		return HttpRequestResult(GetLastError(), 0);
	}

	BOOL bRequestSent = HttpSendRequestW(hRequest, NULL, 0, NULL, 0);

	if (!bRequestSent) {
		return HttpRequestResult(GetLastError(), 0);
	}

	std::string strResponse;
	const int nBuffSize = 1024;
	char buff[nBuffSize];

	BOOL bKeepReading = true;
	DWORD dwBytesRead = -1;

	while (bKeepReading && dwBytesRead != 0) {
		bKeepReading = InternetReadFile(hRequest, buff, nBuffSize, &dwBytesRead);
		strResponse.append(buff, dwBytesRead);
	}

	DWORD http_status_code = 0;
	DWORD length = sizeof(DWORD);
	HttpQueryInfo(
		hRequest,
		HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
		&http_status_code,
		&length,
		NULL
	);

	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hInternet);

	return HttpRequestResult(0, http_status_code);
}

#else

int HttpClient::http_post(string url) {
	return 200;
}

int HttpClient::http_get(string url) {
	return 200;
}

#endif
