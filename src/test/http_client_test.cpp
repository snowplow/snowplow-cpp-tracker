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

#include "catch.hpp"
#include "../http_client.hpp"

TEST_CASE("http_client") {
	// need to come back to this
	//REQUIRE(HttpClient::http_get("") == 200);
	//REQUIRE(HttpClient::http_post("") == 200);

	SECTION("URL cracking works for the current build target") {
		HttpClient::CrackedUrl c = HttpClient::crackUrl("http://google.com/search");
		REQUIRE(c.is_valid == true);
		REQUIRE(c.error_code == 0);
		REQUIRE(c.hostname == "google.com");
		REQUIRE(c.path == "/search");
		REQUIRE(c.port == -1);
		REQUIRE(c.is_https == false);
	}

	SECTION("URL cracking can distinguish between http and https urls") {
		HttpClient::CrackedUrl c = HttpClient::crackUrl("https://google.com/search");
		REQUIRE(c.is_valid == true);
		REQUIRE(c.error_code == 0);
		REQUIRE(c.hostname == "google.com");
		REQUIRE(c.path == "/search");
		REQUIRE(c.port == -1);
		REQUIRE(c.is_https == true);
	}

	SECTION("URL cracking can decipher port specifications") {
		HttpClient::CrackedUrl c = HttpClient::crackUrl("http://google.com:8080/search");
		REQUIRE(c.hostname == "google.com");
		REQUIRE(c.port == 8080);
	}

	SECTION("URL cracking defaults hosts to http if no protocol (e.g. http://) is specified") {
		HttpClient::CrackedUrl c = HttpClient::crackUrl("google.com");
		REQUIRE(c.is_valid == true);
		REQUIRE(c.hostname == "google.com");
		REQUIRE(c.is_https == false);
	}

	SECTION("Tracker agent is set") {
		#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		REQUIRE(HttpClient::TRACKER_AGENT == "Snowplow C++ Tracker (Win32)");
		#endif // need to add in other targets here!	
	}

	SECTION("Invalid url triggers exception for get") {
		// TBC - until I can think of something better to return (a message, or defined error codes)
		bool arg_exception_occurred = false;
		try {
			HttpClient::http_get("not a valid url");
		}
		catch (invalid_argument x) {
			arg_exception_occurred = true;
		}
		REQUIRE(arg_exception_occurred == true);
	}
}
