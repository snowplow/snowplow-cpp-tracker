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

#ifndef EVENT_H
#define EVENT_H

#include "../payload/self_describing_json.hpp"
#include "../payload/event_payload.hpp"
#include "../subject.hpp"
#include <string>
#include <vector>

namespace snowplow {

using std::string;
using std::vector;
using std::shared_ptr;

/**
 * @brief Base class for all event types that concrete event types inherit from.
 */
class Event {
public:
  /**
   * @brief Construct a new Event object
   */
  Event();

  /**
   * @return vector<SelfDescribingJson> Custom event context
   */
  vector<SelfDescribingJson> get_context() const;

  /**
   * @return unsigned long long* Pointer to user-defined Unix timestamp or NULL if not set
   * 
   * Overrides automatically assigned timestamp.
   */
  unsigned long long *get_true_timestamp() const;

  /**
   * @brief Replace the custom context of the event with a new vector of self-describing JSONs.
   * 
   * @param context New custom context
   */
  void set_context(const vector<SelfDescribingJson> &context);

  /**
   * @brief Set the user-defined Unix timestamp (in ms) to the given pointer or NULL.

   * The true timestamp overrides the automatically assigned device timestamp.
   * 
   * @param true_timestamp Pointer to true timestamp or NULL
   */
  void set_true_timestamp(unsigned long long *true_timestamp);

  /**
   * @brief Set the optional subject object to supply additional information to the event.
   * 
   * @param subject Shared pointer to Subject instance
   */
  void set_subject(shared_ptr<Subject> subject);

protected:
  /**
   * @brief This function is overriden by concrete event classes and returns payload with properties for the event types.
   * 
   * @param use_base64 Whether to enable base 64 encoding for self-describing event body
   * @return EventPayload Payload with the custom properties for the event type
   */
  virtual EventPayload get_custom_event_payload(bool use_base64) const = 0;

  /**
   * @brief Helper function to construct payload for a self-describing event given the self-describing JSON.
   * 
   * @param event Self-describing JSON with the event schema and data
   * @param use_base64 Whether to enable base 64 encoding for self-describing event body
   * @return EventPayload Event payload
   */
  EventPayload get_self_describing_event_payload(const SelfDescribingJson &event, bool use_base64) const;

private:
  EventPayload get_payload(bool use_base64) const;
  shared_ptr<Subject> get_subject() const;

  unsigned long long *m_true_timestamp;
  vector<SelfDescribingJson> m_context;
  shared_ptr<Subject> m_subject;

  friend class Tracker;
};
} // namespace snowplow

#endif
