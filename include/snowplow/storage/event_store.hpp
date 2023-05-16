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

#ifndef EVENT_STORE_H
#define EVENT_STORE_H

#include "event_row.hpp"
#include <list>

namespace snowplow {

using std::list;

/**
 * @brief Storage interface used by the Emitter to store and access events.
 *
 * You may implement your own storage or make use of the `SqliteStorage` provided by the tracker.
 */
struct EventStore {
  /**
   * @brief Insert event payload into event queue.
   * 
   * @param payload Event payload to store
   */
  virtual void add_event(const Payload &payload) = 0;

  /**
   * @brief Retrieve event rows from event queue up to the given limit.
   * 
   * @param event_list Output event list to add event rows to
   * @param number_to_get Maximum number of events to retrieve
   */
  virtual void get_event_rows_batch(list<EventRow> *event_list, int number_to_get) = 0;

  /**
   * @brief Remove event rows with the given IDs.
   * 
   * @param id_list List of event row IDs to remove
   */
  virtual void delete_event_rows_with_ids(const list<int> &id_list) = 0;
};
} // namespace snowplow

#endif
