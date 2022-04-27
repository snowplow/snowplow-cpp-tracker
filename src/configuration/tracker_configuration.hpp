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

#ifndef TRACKER_CONFIGURATION_H
#define TRACKER_CONFIGURATION_H

#include <string>

using std::shared_ptr;
using std::string;

namespace snowplow {
/**
 * @brief Configuration object containing settings used to initialize a Snowplow tracker.
 * 
 * The tracker namespace is the only required property.
 */
class TrackerConfiguration {
public:
  /**
   * @brief Construct a configuration with settings for the Tracker.
   * 
   * @param name_space The name of the tracker instance attached to every event (required).
   * @param app_id Application ID (defaults to empty string).
   * @param platform The platform the Tracker is running on, can be one of: web, mob, pc, app, srv, tv, cnsl, iot (defaults to srv).
   */
  TrackerConfiguration(const string &name_space, const string &app_id = SNOWPLOW_DEFAULT_APP_ID, const string &platform = SNOWPLOW_DEFAULT_PLATFORM) : m_namespace(name_space), m_app_id(app_id), m_platform(platform), m_use_base64(true), m_desktop_context(true) {}

  /**
   * @brief Set whether to use base64 encoding in events (defaults to true).
   * 
   * @param use_base64 Whether to enable base 64 encoding (defaults to true).
   */
  void set_use_base64(bool use_base64) { m_use_base64 = use_base64; }

  /**
   * @brief Set whether to add desktop context entity to events (defaults to true).
   * 
   * @param desktop_context Whether to add a desktop_context, which gathers information about the device the tracker is running on, to each event (defaults to true).
   */
  void set_desktop_context(bool desktop_context) { m_desktop_context = desktop_context; }

  /**
   * @return string Tracker namespace.
   */
  string get_namespace() const { return m_namespace; }

  /**
   * @return string Application ID (or empty string if not set).
   */
  string get_app_id() const { return m_app_id; }
  
  /**
   * @return string Platform that the tracker is running on.
   */
  string get_platform() const { return m_platform; }

  /**
   * @return bool Whether to use base64 encoding
   */
  bool get_use_base64() const { return m_use_base64; }

  /**
   * @return bool Whether to append a desktop context entity to events.
   */
  bool get_desktop_context() const { return m_desktop_context; }

private:
  string m_namespace;
  string m_app_id;
  string m_platform;
  bool m_use_base64;
  bool m_desktop_context;
};
} // namespace snowplow

#endif
