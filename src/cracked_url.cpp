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

#include "cracked_url.hpp"

using namespace snowplow;
using std::regex;
using std::regex_match;
using std::smatch;
using std::stringstream;

CrackedUrl::CrackedUrl(const string &url) {
  string cleaned_url = url;
  if (regex_match(cleaned_url, regex("^https?://.+")) == false) {
    cleaned_url = string("http://") + url;
  }

  regex r_host("(https?)://([^\\s\\.]+\\.[^\\s/]+)(/.*)?");
  regex r_hostname_port("([^:]+):(\\d+)");
  smatch match;

  if (regex_search(cleaned_url, match, r_host)) {
    string protocol = match.str(1);
    string hostname_port = match.str(2);
    this->path = match.str(3);

    smatch host_match;
    if (regex_search(hostname_port, host_match, r_hostname_port)) {
      this->hostname = host_match.str(1);
      string port = host_match.str(2);
      this->port = stoi(port);
      this->use_default_port = false;
    } else {
      this->hostname = hostname_port; // it's just a hostname
      this->port = 0;
      this->use_default_port = true;
    }

    this->is_https = protocol == "https";
    this->error_code = 0;
    this->is_valid = true;
  } else {
    this->error_code = -1;
    this->is_valid = false;
  }
}

string CrackedUrl::get_hostname() {
  return this->hostname;
}

string CrackedUrl::get_path() {
  return this->path;
}

bool CrackedUrl::get_is_https() {
  return this->is_https;
}

bool CrackedUrl::get_is_valid() {
  return this->is_valid;
}

int CrackedUrl::get_error_code() {
  return this->error_code;
}

unsigned int CrackedUrl::get_port() {
  return this->port;
}

bool CrackedUrl::get_use_default_port() {
  return this->use_default_port;
}

string CrackedUrl::to_string() {
  stringstream s;

  if (this->get_is_https()) {
    s << "https://";
  } else {
    s << "http://";
  }

  s << this->get_hostname();

  if (!this->get_use_default_port()) {
    s << ":" << std::to_string(this->get_port());
  }

  s << this->get_path();

  return s.str();
}
