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

#ifndef NETWORK_CONFIGURATION_H
#define NETWORK_CONFIGURATION_H

#include <memory>
#include <string>
#include "../http/http_enums.hpp"

using std::shared_ptr;
using std::string;

namespace snowplow {
/**
 * @brief Configuration object containing Snowplow collector settings used to initialize an emitter.
 */
class NetworkConfiguration {
public:
  /**
   * @brief Construct a new configuration.
   * 
   * @param collector_url Full URL of the Snowplow collector including the protocol (or defaults to HTTPS if protocol not present).
   * @param method HTTP method to use when sending events to collector – GET or POST.
   */
  NetworkConfiguration(const string &collector_url, Method method = POST);

  /**
   * @brief Get the collector hostname without the protocol.
   * 
   * @return string Hostname part of the collector URL
   */
  string get_collector_hostname() const { return m_collector_hostname; }

  /**
   * @brief Get the HTTP method.
   * 
   * @return Method HTTP method to use when accessing the collector.
   */
  Method get_method() const { return m_method; }

  /**
   * @brief Get the HTTP protocol (HTTP or HTTPS).
   * 
   * @return Protocol HTTP protocol to use when accessing the collector.
   */
  Protocol get_protocol() const { return m_protocol; }

private:
  string m_collector_hostname;
  Method m_method;
  Protocol m_protocol;
};
} // namespace snowplow

#endif
