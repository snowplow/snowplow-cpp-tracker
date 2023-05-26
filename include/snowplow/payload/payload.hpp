/*
Copyright (c) 2023 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#ifndef PAYLOAD_H
#define PAYLOAD_H

#include "../detail/base64/base64.hpp"
#include "../thirdparty/json.hpp"
#include <map>
#include <string>

namespace snowplow {

using std::map;
using std::string;
using json = nlohmann::json;

/**
 * @brief Snowplow event payload with event properties.
 */
class Payload {
private:
  map<string, string> m_pairs;

public:
  ~Payload();

  /**
   * @brief Add a property to the payload.
   *
   * @param key Property key
   * @param value Property value
   */
  void add(const string &key, const string &value);

  /**
   * @brief Add a map of properties to the payload.
   *
   * @param pairs Key-value pairs
   */
  void add_map(map<string, string> pairs);

  /**
   * @brief Add properties from another payload.
   *
   * @param p Payload to add values from
   */
  void add_payload(const Payload &p);

  /**
   * @brief Add self-describing JSON data to the payload.
   *
   * @param j Self-describing JSON
   * @param base64Encode Should the data be base64 encoded
   * @param encoded Key for encoded data
   * @param not_encoded Key for not-encoded data
   */
  void add_json(const json &j, bool base64Encode, const string &encoded, const string &not_encoded);

  /**
   * @brief Get the payload key-value pairs.
   *
   * @return Payload as key-value pairs
   */
  map<string, string> get() const;
};
} // namespace snowplow

#endif
