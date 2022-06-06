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

#ifndef TRACKER_PAYLOAD_H
#define TRACKER_PAYLOAD_H

#include "payload.hpp"
#include <string>

using std::string;

namespace snowplow {
/**
 * @brief Payload with event properties that is created for tracked events.
 * 
 * In contrast with the base `Payload` class, `EventPayload` contains event ID and device timestamp properties.
 */
class EventPayload : public Payload {
public:
  /**
   * @brief Construct a new Event Payload and initializes the event ID and device timestamp.
   */
  EventPayload();

  /**
   * @brief Get the event ID
   * 
   * @return string Automatically generated event ID
   */
  string get_event_id() const;

  /**
   * @brief Get the device created timestamp
   * 
   * @return unsigned long long Automatically assigned Unix timestamp
   */
  unsigned long long get_timestamp() const;

private:
  unsigned long long m_timestamp;
  string m_event_id;
};
} // namespace snowplow

#endif
