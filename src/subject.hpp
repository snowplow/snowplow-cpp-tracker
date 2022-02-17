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

#ifndef SUBJECT_H
#define SUBJECT_H

#include <map>
#include <string>
#include "payload.hpp"
#include "constants.hpp"

using std::map;
using std::string;

/**
 * @brief Defines additional information about your application's environment, current user and so on, to be sent to with each tracked event.
 */
class Subject {
private:
  Payload m_payload;

public:
  /**
   * @brief Set the business user ID string
   * 
   * @param user_id Business user ID
   */
  void set_user_id(const string & user_id);

  /**
   * @brief Set the device screen resolution
   * 
   * @param width Device screen resolution width
   * @param height Device screen resolution height
   */
  void set_screen_resolution(int width, int height);

  /**
   * @brief Set the device viewport dimensions
   * 
   * @param width Device viewport width
   * @param height Device viewport height
   */
  void set_viewport(int width, int height);

  /**
   * @brief Set the bit depth of the device’s color palette for displaying images
   * 
   * @param depth Device color depth
   */
  void set_color_depth(int depth);

  /**
   * @brief Set the user’s timezone.
   * 
   * @param timezone User's timezone (e.g., "Europe/London")
   */
  void set_timezone(const string & timezone);

  /**
   * @brief Set the user's language
   * 
   * @param language User's language (e.g., "en")
   */
  void set_language(const string & language);

  /**
   * @brief Set the useragent string for the event
   * 
   * @param user_agent Standard formatted useragent string (e.g., "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_4)...")
   */
  void set_useragent(const string & user_agent);

  /**
   * @brief Get the subject properties as a map of strings
   * 
   * @return map<string, string> Subject properties to be added to events
   */
  map<string, string> get_map();
};

#endif
