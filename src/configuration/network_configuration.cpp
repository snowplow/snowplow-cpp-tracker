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

#include "network_configuration.hpp"
#include <algorithm>

using namespace snowplow;

using std::transform;

NetworkConfiguration::NetworkConfiguration(const string &collector_url, Method method) {
  m_method = method;

  string collector_url_lower = collector_url;
  transform(collector_url_lower.begin(), collector_url_lower.end(), collector_url_lower.begin(), ::tolower);

  const string expected_http = "http://";
  const string expected_https = "https://";

  // starts with http://
  if (expected_http.size() <= collector_url_lower.size() && equal(expected_http.begin(), expected_http.end(), collector_url_lower.begin())) {
    m_protocol = HTTP;
    m_collector_hostname = collector_url.substr(expected_http.size());
  }
  // starts with https://
  else if (expected_https.size() <= collector_url_lower.size() && equal(expected_https.begin(), expected_https.end(), collector_url_lower.begin())) {
    m_protocol = HTTPS;
    m_collector_hostname = collector_url.substr(expected_https.size());
  }
  // doesn't contain a protocol, default to HTTPS
  else {
    m_protocol = HTTPS;
    m_collector_hostname = collector_url;
  }
}
