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

#ifndef SELF_DESCRIBING_EVENT_H
#define SELF_DESCRIBING_EVENT_H

#include "event.hpp"

namespace snowplow {
/**
 * @brief Event to track custom information that does not fit into the out-of-the box events.
 *
 * Self-describing events are a [data structure based on JSON Schemas](https://docs.snowplow.io/docs/understanding-tracking-design/understanding-schemas-and-validation/)
 * and can have arbitrarily many fields.
 * To define your own custom self-describing event, you must create a JSON schema for that
 * event and upload it to an [Iglu Schema Repository](https://github.com/snowplow/iglu) using
 * [igluctl](https://docs.snowplow.io/docs/open-source-components-and-applications/iglu/)
 * (or if a Snowplow BDP customer, you can use the
 * [Snowplow BDP Console UI](https://docs.snowplow.io/docs/understanding-tracking-design/managing-data-structures/)
 * or [Data Structures API](https://docs.snowplow.io/docs/understanding-tracking-design/managing-data-structures-via-the-api-2/)).
 * Snowplow uses the schema to validate that the JSON containing the event properties is well-formed.
 */
class SelfDescribingEvent : public Event {
public:
  /**
   * @brief Construct a new Self Describing Event object
   *
   * @param event Main properties of the self-describing event including its schema and body
   */
  SelfDescribingEvent(const SelfDescribingJson &event);

  /**
   * @brief Main properties of the self-describing event including its schema and body
   */
  SelfDescribingJson event; // required

protected:
  EventPayload get_custom_event_payload(bool use_base64) const override;
};
} // namespace snowplow

#endif
