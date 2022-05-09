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

#ifndef EMITTER_H
#define EMITTER_H

#include <string>
#include <sstream>
#include <condition_variable>
#include <future>
#include <thread>
#include <algorithm>
#include "../constants.hpp"
#include "../utils.hpp"
#include "../storage/event_store.hpp"
#include "../payload/payload.hpp"
#include "../payload/self_describing_json.hpp"
#include "../cracked_url.hpp"
#include "../http/http_request_result.hpp"
#include "../http/http_client.hpp"
#include "../configuration/network_configuration.hpp"
#include "../configuration/emitter_configuration.hpp"
#include "../emitter/emit_status.hpp"
#include "retry_delay.hpp"
#include "../http/http_enums.hpp"

using std::string;
using std::thread;
using std::condition_variable;
using std::mutex;
using std::unique_ptr;
using std::list;
using std::shared_ptr;

namespace snowplow {

/**
 * @brief Emitter is responsible for sending events to a Snowplow Collector.
 * 
 * Once the emitter receives an event from the Tracker a few things start to happen:
 * 
 * 1. The event is added to a local Sqlite3 database (blocking execution)
 * 2. A long running daemon thread is started which will continue to send events as long as they can be found in the database (asynchronous)
 * 3. The emitter loop will grab a range of events from the database up until the SendLimit
 * 4. The emitter will send all of these events as determined by the Request, Protocol and ByteLimits
 *    - Each request is sent in its thread.
 * 5. Once sent it will process the results of all the requests sent and will remove all successfully sent events from the database
 * 
 * You may optionally configure the HTTP client to be used to make HTTP requests to the collector.
 * This is done by passing a unique pointer to a class inheriting from `HttpClient` that the Emitter will take ownership of.
 * If not configured, the Emitter will use the built-in `HttpClientWindows` on Windows, `HttpClientApple` on Apple operating systems, and `HttpClientCurl` on other Unix systems.
 */
class Emitter {
public:
  /**
   * @brief Construct a new Emitter using the configuration
   * 
   * @param network_config Network configuration to access the collector
   * @param emitter_config Configuration settings for the emitter
   */
  Emitter(const NetworkConfiguration &network_config, EmitterConfiguration &emitter_config);

  /**
   * @brief Construct a new Emitter object with a custom HTTP client
   *
   * @param event_store Defines the database to use for event queue
   * @param uri The URI to send events to
   * @param method The request type to use (GET or POST)
   * @param protocol The protocol to use (HTTP or HTTPS)
   * @param batch_size The maximum amount of events to send at a time
   * @param byte_limit_post The byte limit when sending a POST request
   * @param byte_limit_get The byte limit when sending a GET request
   * @param http_client Unique pointer to a custom HTTP client to send GET and POST requests with
   */
  Emitter(shared_ptr<EventStore> event_store, const string & uri, Method method = POST, Protocol protocol = HTTPS, int batch_size = SNOWPLOW_EMITTER_DEFAULT_BATCH_SIZE, 
    int byte_limit_post = SNOWPLOW_EMITTER_DEFAULT_BYTE_LIMIT_POST, int byte_limit_get = SNOWPLOW_EMITTER_DEFAULT_BYTE_LIMIT_GET, unique_ptr<HttpClient> http_client = nullptr);

  ~Emitter();

  /**
   * @brief Starts a long running daemon thread for sending events. Triggered automatically by the tracker.
   */
  virtual void start();

  /**
   * @brief Stops the long running daemon thread for sending events. Triggered automatically by the tracker.
   */
  virtual void stop();

  /**
   * @brief Adds an event to the database for sending. Triggered by tracker.
   * 
   * @param payload Event payload
   */
  virtual void add(Payload payload);

  /**
   * @brief Force send queued events.
   */
  virtual void flush();

  /**
   * @brief Get broken down collector URL.
   * 
   * @return CrackedUrl 
   */
  CrackedUrl get_cracked_url() const { return m_url; }

  /**
   * @brief Get the HTTP method.
   * 
   * @return Method HTTP method used for sending events to Collector
  */
  Method get_method() const { return m_method; }

  /**
   * @brief Get the batch size.
   * 
   * @return unsigned int The maximum amount of events to send at a time
   */
  unsigned int get_batch_size() const { return m_batch_size; }

  /**
   * @brief Get the byte limit for GET.
   * 
   * @return unsigned int The byte limit when sending a GET request
   */
  unsigned int get_byte_limit_get() const { return m_byte_limit_get; }

  /**
   * @brief Get the byte limit for POST.
   * 
   * @return unsigned int The byte limit when sending a POST request
   */
  unsigned int get_byte_limit_post() const { return m_byte_limit_post; }

  /**
   * @brief Check if the Emitter is started.
   * 
   * @return true if Emitter is running
   */
  bool is_running();

  /**
   * @brief Set a callback to call after emit requests are made with the resulting emit status.
   * 
   * To subscribe to multiple emit statuses, use binary operations such as `EmitStatus::FAILED_WILL_RETRY | EmitStatus::FAILED_WONT_RETRY`.
   * Calling this function overwrites any previously set callbacks.
   * The callback can't be changed when the Emitter is running.
   * The callback will be fired in a new thread.
   * 
   * @param callback Callback function
   * @param emit_status Emit status to trigger the callback for
   */
  void set_request_callback(const EmitterCallback &callback, EmitStatus emit_status);

  /**
   * @brief Set a custom retry rule for when the HTTP status code is received in emit response from Collector.
   * 
   * This overrides default behavior for HTTP status codes greater than 300.
   * The custom retry rules can't be changed when the Emitter is running.
   * 
   * @param http_status_code HTTP status code
   * @param retry Whether events should be retried or not
   */
  void set_custom_retry_for_status_code(int http_status_code, bool retry);

private:
  CrackedUrl m_url;
  Method m_method;
  shared_ptr<EventStore> m_event_store;
  unique_ptr<HttpClient> m_http_client;
  unsigned int m_batch_size;
  unsigned int m_byte_limit_get;
  unsigned int m_byte_limit_post;

  thread m_daemon_thread;
  condition_variable m_check_db;
  condition_variable m_check_fin;
  mutex m_flush_fin;
  mutex m_db_select;
  mutex m_run_check;
  bool m_running;
  EmitterCallback m_callback;
  EmitStatus m_callback_emit_status;
  map<int, bool> m_custom_retry_for_status_codes;
  RetryDelay m_retry_delay;

  void run();
  void do_send(const list<EventRow> &event_rows, list<HttpRequestResult> *results);
  string build_post_data_json(list<Payload> payload_list);
  string get_collector_url(const string &uri, Protocol protocol, Method method) const;
  void trigger_callbacks(const list<int> &success_row_ids, const list<int> &failed_will_retry_row_ids, const list<int> &failed_wont_retry_row_ids, const list<EventRow> &event_rows) const;
  void execute_callback(const list<string> &event_ids, EmitStatus emit_status) const;
};
} // namespace snowplow

#endif
