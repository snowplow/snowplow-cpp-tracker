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

#ifndef TIMING_H
#define TIMING_H

#include "event.hpp"

namespace snowplow {

const string SNOWPLOW_SCHEMA_USER_TIMINGS = "iglu:com.snowplowanalytics.snowplow/timing/jsonschema/1-0-0";

/**
 * @brief Event used to track user timing events such as how long resources take to load.
 *
 * Schema: iglu:com.snowplowanalytics.snowplow/timing/jsonschema/1-0-0
 */
class TimingEvent : public Event {
public:
  /**
   * @brief Construct a new Timing Event object
   *
   * @param category Defines the timing category.
   * @param variable Defines the timing variable measured.
   * @param timing Represents the time.
   */
  TimingEvent(const string &category, const string &variable, unsigned long long timing);

  /**
   * @brief Defines the timing category.
   */
  string category; // required

  /**
   * @brief Defines the timing variable measured.
   */
  string variable; // required

  /**
   * @brief Represents the time.
   */
  unsigned long long timing; // required

  /**
   * @brief An optional string to further identify the timing event.
   */
  string *label;

protected:
  EventPayload get_custom_event_payload(bool use_base64) const override;
};
} // namespace snowplow

#endif
