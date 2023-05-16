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

#ifndef SNOWPLOW_H
#define SNOWPLOW_H

/**
 * @brief This file provides the single header to import when using the Snowplow tracker in your code.
 */

#include "client_session.hpp"
#include "subject.hpp"
#include "tracker.hpp"

// emitter
#include "emitter/emitter.hpp"

// storage
#include "storage/event_row.hpp"
#include "storage/event_store.hpp"
#include "storage/session_store.hpp"
#include "storage/sqlite_storage.hpp"

// http
#include "http/http_enums.hpp"
#include "http/http_client.hpp"
#include "http/http_client_apple.hpp"
#include "http/http_client_curl.hpp"
#include "http/http_client_windows.hpp"

// payload
#include "payload/payload.hpp"
#include "payload/self_describing_json.hpp"

// events
#include "events/event.hpp"
#include "events/screen_view_event.hpp"
#include "events/self_describing_event.hpp"
#include "events/structured_event.hpp"
#include "events/timing_event.hpp"

// configuration
#include "configuration/tracker_configuration.hpp"
#include "configuration/network_configuration.hpp"
#include "configuration/emitter_configuration.hpp"
#include "configuration/session_configuration.hpp"

#include <map>
#include <mutex>

namespace snowplow {

using std::shared_ptr;
using std::map;
using std::mutex;

/**
 * @brief Static entry point to instance a new Snowplow tracker or manage existing trackers.
 * 
 * New trackers can be initialized through the `Snowplow::create_tracker()` methods.
 * The function accepts various combinations of parameters that can be grouped into 2 kinds: with default configuration, and with custom configuration.
 * 
 * To create a tracker with default configuration, you may directly pass only the required properties as so:
 * 
 *     auto tracker = Snowplow::create_tracker("ns", "https://collector.com", POST, "sp.db");
 * 
 * This will create a tracker with namespace "ns", given collector URI, sending events by HTTP POST, and an SQLite database used for event queue and session tracking storage.
 * You may further supply a Subject instance to add user and device information, or disable session tracking (see `ClientSession` for more information about session tracking).
 * 
 * To create a tracker with custom configuration, you may instantiate and pass custom configuration objects (`TrackerConfiguration`, `NetworkConfiguration`, `EmitterConfiguration`, and `SessionConfiguration`):
 * 
 *    TrackerConfiguration tracker_config("namespace", "app-id", "mob");
 *    tracker_config.set_desktop_context(true);
 *    
 *    NetworkConfiguration network_config(uri, POST);
 *    
 *    EmitterConfiguration emitter_config("sp.db");
 *    emitter_config.set_batch_size(500);
 *    
 *    SessionConfiguration session_config("sp.db", 5000, 5000);
 *    
 *    auto tracker = Snowplow::create_tracker(tracker_config, network_config, emitter_config, session_config);
 * 
 * Once initialized through `Snowplow::create_tracker(...)`, you can retrieve the tracker by it's namespace using `Snowplow::get_tracker(namespace)`.
 * You can also use `Snowplow::get_default_tracker()` to get the first created tracker or the one set by `Snowplow::set_default_tracker(tracker)`.
 * 
 * Finally, you can remove tracker references by `Snowplow::remove_tracker(tracker)`.
 * Trackers will be destroyed once all remaining references to them are deleted.
 */
class Snowplow {
public:
  /**
   * @brief Create a tracker with default configuration and with optional session tracking.
   * 
   * Uses an SQLite storage database (with `db_name` relative path) for event queue and session storage (if `session_tracking` is enabled).
   *
   * @param name_space The unique name of the tracker instance that is also to attached to every event.
   * @param collector_url Full URL of the Snowplow collector including the protocol (or defaults to HTTPS if protocol not present).
   * @param method HTTP method to use when sending events to collector – GET or POST.
   * @param db_name Relative path to the SQLite database to use as event and session storage.
   * @param subject Optional subject instance with user and device information.
   * @param session_tracking Whether to track client sessions or not (defaults to true).
   * @return shared_ptr<Tracker> Shared pointer to the initialized tracker.
   */
  static shared_ptr<Tracker> create_tracker(const string &name_space, const string &collector_url, Method method, const string &db_name, shared_ptr<Subject> subject = nullptr, bool session_tracking = true);

  /**
   * @brief Create a tracker with default configuration and without session tracking.
   * 
   * Uses a custom `EventStore` implementation for storing event queue.
   * 
   * @param name_space The unique name of the tracker instance that is also to attached to every event.
   * @param collector_url Full URL of the Snowplow collector including the protocol (or defaults to HTTPS if protocol not present).
   * @param method HTTP method to use when sending events to collector – GET or POST.
   * @param event_store Database to use for event queue.
   * @param subject Optional subject instance with user and device information.
   * @return shared_ptr<Tracker> Shared pointer to the initialized tracker.
   */
  static shared_ptr<Tracker> create_tracker(const string &name_space, const string &collector_url, Method method, shared_ptr<EventStore> event_store, shared_ptr<Subject> subject = nullptr);

  /**
   * @brief Create a tracker with default configuration and with session tracking.
   * 
   * Uses a custom `EventStore` and `SessionStore` implementation for storing event queue and current session.
   * 
   * @param name_space The unique name of the tracker instance that is also to attached to every event.
   * @param collector_url Full URL of the Snowplow collector including the protocol (or defaults to HTTPS if protocol not present).
   * @param method HTTP method to use when sending events to collector – GET or POST.
   * @param event_store Database to use for event queue.
   * @param session_store Database to use for session store.
   * @param subject Optional subject instance with user and device information.
   * @return shared_ptr<Tracker> Shared pointer to the initialized tracker.
   */
  static shared_ptr<Tracker> create_tracker(const string &name_space, const string &collector_url, Method method, shared_ptr<EventStore> event_store, shared_ptr<SessionStore> session_store, shared_ptr<Subject> subject = nullptr);

  /**
   * @brief Create a tracker with custom configuration and without session tracking.
   * 
   * @param tracker_config Configuration object with settings for the tracker.
   * @param network_config Configuration object with network settings for the Emitter.
   * @param emitter_config Configuration object with additional settings for the Emitter.
   * @param subject Optional subject instance with user and device information.
   * @return shared_ptr<Tracker> Shared pointer to the initialized tracker.
   */
  static shared_ptr<Tracker> create_tracker(const TrackerConfiguration &tracker_config, NetworkConfiguration &network_config, const EmitterConfiguration &emitter_config, shared_ptr<Subject> subject = nullptr);

  /**
   * @brief Create a tracker with custom configuration and with session tracking.
   * 
   * @param tracker_config Configuration object with settings for the tracker.
   * @param network_config Configuration object with network settings for the Emitter.
   * @param emitter_config Configuration object with additional settings for the Emitter.
   * @param session_config Configuration object for client session management.
   * @param subject Optional subject instance with user and device information.
   * @return shared_ptr<Tracker> Shared pointer to the initialized tracker.
   */
  static shared_ptr<Tracker> create_tracker(const TrackerConfiguration &tracker_config, NetworkConfiguration &network_config, EmitterConfiguration &emitter_config, SessionConfiguration &session_config, shared_ptr<Subject> subject = nullptr);

  /**
   * @brief Register a new tracker that wasn't created using the `Snowplow::create_tracker` methods.
   * 
   * To be used when a `Tracker` instance is initialized manually and not through the `Snowplow` interface.
   *
   * @param tracker Tracker to register.
   */
  static void register_tracker(shared_ptr<Tracker> tracker);

  /**
   * @brief Get an initialized tracker instance by it's namespace.
   * 
   * @param name_space The namespace of the tracker instance to retrieve.
   * @return shared_ptr<Tracker> Shared pointer to the tracker or nullptr if tracker with the namespace not found.
   */
  static shared_ptr<Tracker> get_tracker(const string &name_space);

  /**
   * @brief Remove reference to the tracker from the registered trackers.
   * 
   * The tracker will be destroyed when all it's references are destroyed.
   *
   * @param tracker Tracker to remove.
   * @return true Tracker was found among the registered trackers and removed.
   * @return false Tracker was not previously registered and thus wasn't removed.
   */
  static bool remove_tracker(shared_ptr<Tracker> tracker);

  /**
   * @brief Remove reference to tracker by it's namespace.

   * The tracker will be destroyed when all it's references are destroyed.
   *
   * @param name_space Namespace of the tracker to remove.
   * @return true Tracker was found among the registered trackers and removed.
   * @return false Tracker was not previously registered and thus wasn't removed.
   */
  static bool remove_tracker(const string &name_space);

  /**
   * @brief Set the default tracker to be retrieved using the `Snowplow::get_default_tracker()` method.
   *
   * @param tracker The tracker to use.
   */
  static void set_default_tracker(shared_ptr<Tracker> tracker);

  /**
   * @brief Get the default tracker.
   * 
   * Default tracker is the first initialized tracker or the one set by `Snowplow::set_default_tracker()`.
   * 
   * It may also be a nullptr in case there are no initialized trackers or the default tracker is removed.
   *
   * @return shared_ptr<Tracker> Default tracker or nullptr.
   */
  static shared_ptr<Tracker> get_default_tracker();

private:
  static map<string, shared_ptr<Tracker>> m_trackers;
  static shared_ptr<Tracker> m_default_tracker;
  static mutex m_tracker_get;
};
} // namespace snowplow

#endif
