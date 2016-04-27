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

int HttpClient::http_post(string url) {
	return 200; // all ok
}

int HttpClient::http_get(string url) {
	HINTERNET hInternet = InternetOpenW(L"SNOWPLOW", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

	if (hInternet == NULL) {
		return GetLastError();
	}

	HINTERNET hConnect = InternetConnectW(hInternet, L"63f27ff3.ngrok.io", 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);

	if (hConnect == NULL) {
		InternetCloseHandle(hInternet);
		return GetLastError();
	}

	const wchar_t* parrAcceptTypes[] = { L"text/*", NULL };
	HINTERNET hRequest = HttpOpenRequestW(hConnect, L"GET", L"/i?e=pv", NULL, NULL, parrAcceptTypes, 0 | INTERNET_FLAG_RELOAD, 0);

	if (hRequest == NULL) {
		InternetCloseHandle(hInternet);
		InternetCloseHandle(hConnect);
		return GetLastError();
	}

	BOOL bRequestSent = HttpSendRequestW(hRequest, NULL, 0, NULL, 0);

	if (!bRequestSent) {
		return GetLastError();
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

	DWORD statusCode = 0;
	DWORD length = sizeof(DWORD);
	HttpQueryInfo(
		hRequest,
		HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
		&statusCode,
		&length,
		NULL
	);

	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hInternet);

	return statusCode;
}