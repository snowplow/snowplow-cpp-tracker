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

#ifndef SESSION_STORE_H
#define SESSION_STORE_H

#include "../../include/json.hpp"
#include <list>

using std::list;
using std::unique_ptr;
using json = nlohmann::json;

namespace snowplow {
/**
 * @brief Storage interface used by the ClientSession to store and access sessions.
 *
 * You may implement your own storage or make use of the `SqliteStorage` provided by the tracker.
 */
struct SessionStore {
  /**
   * @brief Return the current session.
   * 
   * @return Pointer to session or nullptr if it doesn't exist
   */
  virtual unique_ptr<json> get_session() = 0;

  /**
   * @brief Insert or replace the session.
   * 
   * @param session_data Data to insert
   */
  virtual void set_session(const json &session_data) = 0;

  /**
   * @brief Remove the current session.
   */
  virtual void delete_session() = 0;
};
} // namespace snowplow

#endif
