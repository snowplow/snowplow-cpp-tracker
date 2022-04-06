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

#ifndef SCREEN_VIEW_EVENT_H
#define SCREEN_VIEW_EVENT_H

#include "event.hpp"

namespace snowplow {
/**
 * @brief Event to track user viewing a screen within the application.
 *
 * Schema for the event: iglu:com.snowplowanalytics.snowplow/screen_view/jsonschema/1-0-0
 */
class ScreenViewEvent : public Event {
public:
  /**
   * @brief Construct a new Screen View Event object
   */
  ScreenViewEvent();

  /**
   * @brief The name of the screen viewed.
   */
  string *name;

  /**
   * @brief The id of screen that was viewed.
   */
  string *id;

protected:
  EventPayload get_custom_event_payload(bool use_base64) const override;
};
} // namespace snowplow

#endif
