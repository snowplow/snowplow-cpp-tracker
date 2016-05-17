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

#include "../vendored/catch.hpp"
#include "../cracked_url.hpp"

TEST_CASE("cracked_url") {
  SECTION("URL cracking works for the current build target") {
    CrackedUrl c("http://google.com/search/tp2");
    REQUIRE(c.get_is_valid() == true);
    REQUIRE(c.get_error_code() == 0);
    REQUIRE(c.get_hostname() == "google.com");
    REQUIRE(c.get_path() == "/search/tp2");
    REQUIRE(c.get_port() == 0);
    REQUIRE(c.get_use_default_port() == true);
    REQUIRE(c.get_is_https() == false);
  }

  SECTION("URL cracking can distinguish between http and https urls") {
    CrackedUrl c("https://google.com/search");
    REQUIRE(c.get_is_valid() == true);
    REQUIRE(c.get_error_code() == 0);
    REQUIRE(c.get_hostname() == "google.com");
    REQUIRE(c.get_path() == "/search");
    REQUIRE(c.get_port() == 0);
    REQUIRE(c.get_use_default_port() == true);
    REQUIRE(c.get_is_https() == true);
  }

  SECTION("URL cracking can decipher port specifications") {
    CrackedUrl c("http://google.com:8080/search");
    REQUIRE(c.get_hostname() == "google.com");
    REQUIRE(c.get_port() == 8080);
    REQUIRE(c.get_use_default_port() == false);
  }

  SECTION("URL cracking defaults hosts to http if no protocol (e.g. http://) is specified") {
    CrackedUrl c("google.com");
    REQUIRE(c.get_is_valid() == true);
    REQUIRE(c.get_hostname() == "google.com");
    REQUIRE(c.get_is_https() == false);
    REQUIRE(c.get_use_default_port() == true);
  }

  SECTION("URL cracking defaults hosts to http if no protocol (e.g. http://) is specified") {
    CrackedUrl c("google.com");
    REQUIRE(c.get_is_valid() == true);
    REQUIRE(c.get_hostname() == "google.com");
    REQUIRE(c.get_is_https() == false);
  }

  SECTION("url cracks multiple slashes and port") {
    CrackedUrl c("http://www.google.com:8080/hello/world");
    REQUIRE(c.get_is_valid() == true);
    REQUIRE(c.get_port() == 8080);
    REQUIRE(c.get_hostname() == "www.google.com");
    REQUIRE(c.get_path() == "/hello/world");
  }

  SECTION("url with dots") {
    CrackedUrl c("http://c91c801c.ngrok.io/com.snowplowanalytics.snowplow/tp2");
    REQUIRE(c.get_is_valid() == true);
    REQUIRE(c.get_hostname() == "c91c801c.ngrok.io");
    REQUIRE(c.get_path() == "/com.snowplowanalytics.snowplow/tp2");
  }
}
