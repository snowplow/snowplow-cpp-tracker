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

#ifndef SELF_DESCRIBING_JSON_H
#define SELF_DESCRIBING_JSON_H

#include <string>
#include "../constants.hpp"
#include "../thirdparty/json.hpp"

namespace snowplow {

using std::string;
using json = nlohmann::json;

/**
 * @brief Self-describing JSON object used for defining self-describing events or custom context entities.
 */
class SelfDescribingJson {
private:
  json m_json;

public:
  /**
   * @brief Construct a new Self Describing Json object
   * 
   * @param schema Iglu schema (e.g., "iglu:com.snowplowanalytics.snowplow/timing/jsonschema/1-0-0")
   * @param data Data payload with unstructured set of properties
   */
  SelfDescribingJson(const string &schema, const json &data);

  /**
   * @brief Destroy the Self Describing Json object
   */
  ~SelfDescribingJson();

  /**
   * @brief Return the content of the self-describing JSON.
   * 
   * @return json Content as a JSON object
   */
  json get() const;

  /**
   * @brief Return the content of the self-describing JSON as string.
   * 
   * @return string Content as a JSON string
   */
  string to_string() const;
};
}

#endif
