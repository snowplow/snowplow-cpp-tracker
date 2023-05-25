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

#ifndef STRUCTURED_EVENT_H
#define STRUCTURED_EVENT_H

#include "event.hpp"

namespace snowplow {
/**
 * @brief  Event to capture custom consumer interactions without the need to define a custom schema.
 */
class StructuredEvent : public Event {
public:
  /**
   * @brief Construct a new Structured Event object
   *
   * @param category Name for the group of objects you want to track e.g. "media", "ecomm".
   * @param action Defines the type of user interaction for the web object.
   */
  StructuredEvent(const string &category, const string &action);

  /**
   * @brief Name for the group of objects you want to track e.g. "media", "ecomm".
   */
  string category; // required

  /**
   * @brief Defines the type of user interaction for the web object.
   *
   * E.g., "play-video", "add-to-basket".
   */
  string action; // required

  /**
   * @brief Identifies the specific object being actioned.
   *
   * E.g., ID of the video being played, or the SKU or the product added-to-basket.
   */
  string *label;

  /**
   * @brief Describes the object or the action performed on it.
   *
   * This might be the quantity of an item added to basket
   */
  string *property;

  /**
   * @brief Quantifies or further describes the user action.
   *
   * This might be the price of an item added-to-basket, or the starting time of the video where play was just pressed.
   */
  double *value;

protected:
  EventPayload get_custom_event_payload(bool use_base64) const override;
};
} // namespace snowplow

#endif
