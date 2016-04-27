
#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <map>
#include <string>
#include <windows.h>
#include <WinInet.h>

#pragma comment (lib, "wininet.lib")

using namespace std;

class HttpClient {
public:
	static int http_post(string);
	static int http_get(string);
};

#endif