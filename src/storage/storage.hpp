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

#ifndef STORAGE_H
#define STORAGE_H

#include "event_row.hpp"
#include "../../include/json.hpp"
#include <list>

using std::list;
using json = nlohmann::json;

namespace snowplow {
/**
 * @brief Storage interface used by the Emitter and ClientSession to store and access events and sessions.
 *
 * You may implement your own storage or make use of the `SqliteStorage` provided by the tracker.
 */
struct Storage {
  /**
   * @brief Insert event payload into event queue.
   * 
   * @param payload Event payload to store
   */
  virtual void insert_payload(const Payload &payload) = 0;

  /**
   * @brief Retrieve all event rows waiting in the event queue.
   * 
   * @param event_list Output event list to add event rows to
   */
  virtual void select_all_event_rows(list<EventRow> *event_list) = 0;

  /**
   * @brief Select event rows from event queue up to the given limit.
   * 
   * @param event_list Output event list to add event rows to
   * @param range Maximum number of events to select
   */
  virtual void select_event_row_range(list<EventRow> *event_list, int range) = 0;

  /**
   * @brief Remove all event rows from the event queue
   */
  virtual void delete_all_event_rows() = 0;

  /**
   * @brief Remove event rows with the given IDs.
   * 
   * @param id_list List of event row IDs to remove
   */
  virtual void delete_event_row_ids(const list<int> &id_list) = 0;

  /**
   * @brief Select all session rows – there should be at most 1.
   * 
   * @param session_list Output list of sessions (of size 0 or 1)
   */
  virtual void select_all_session_rows(list<json>* session_list) = 0;

  /**
   * @brief Insert or replace the session.
   * 
   * @param session_data Data to insert
   */
  virtual void insert_update_session(const json &session_data) = 0;

  /**
   * @brief Remove all sessions.
   */
  virtual void delete_all_session_rows() = 0;
};
} // namespace snowplow

#endif
