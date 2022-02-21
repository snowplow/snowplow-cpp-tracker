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

#ifndef CRACKED_URL_H
#define CRACKED_URL_H

#include <string>
#include <sstream>
#include <regex>

using std::string;

class CrackedUrl {
public:
  CrackedUrl(const string & url);
  string get_hostname();
  string get_path();
  bool get_is_https();
  bool get_is_valid();
  int get_error_code();
  unsigned int get_port();
  bool get_use_default_port();
  string to_string();

private:
  string hostname;
  string path;
  bool is_https;
  bool is_valid;
  int error_code;
  unsigned int port;
  bool use_default_port;
};

#endif
