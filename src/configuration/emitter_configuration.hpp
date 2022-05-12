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

#ifndef EMITTER_CONFIGURATION_H
#define EMITTER_CONFIGURATION_H

#include <string>
#include "../storage/event_store.hpp"
#include "../http/http_client.hpp"
#include "../emitter/emit_status.hpp"
#include "../storage/sqlite_storage.hpp"

using std::shared_ptr;
using std::unique_ptr;
using std::string;
using std::move;
using std::make_shared;

namespace snowplow {

typedef std::function<void(list<string>, EmitStatus)> EmitterCallback;

/**
 * @brief Configuration object containing settings used to initialize a Snowplow emitter.
 */
class EmitterConfiguration {
public:
  /**
   * @brief Construct configuration with an `EventStore` instance (see `SqliteStorage` for for SQLite implementation provided by the tracker).
   * 
   * @param event_store Storage for event queue.
   */
  EmitterConfiguration(shared_ptr<EventStore> event_store);

  /**
   * @brief Construct configuration with SQLite event queue.
   * 
   * @param db_name Relative path to the SQLite database.
   */
  EmitterConfiguration(const string &db_name);

  /**
   * @brief Set the maximum batch size.
   * 
   * @param batch_size The maximum amount of events to send at a time.
   */
  void set_batch_size(int batch_size) { m_batch_size = batch_size; }

  /**
   * @brief Set the byte limit for GET requests.
   * 
   * @param byte_limit_get The byte limit when sending a GET request.
   */
  void set_byte_limit_get(int byte_limit_get) { m_byte_limit_get = byte_limit_get; }

  /**
   * @brief Set the byte limit for POST requests.
   * 
   * @param byte_limit_post The byte limit when sending a POST request.
   */
  void set_byte_limit_post(int byte_limit_post) { m_byte_limit_post = byte_limit_post; }

  /**
   * @brief Set custom HTTP client.
   * 
   * @param http_client Unique pointer to a custom HTTP client to send GET and POST requests with.
   */
  void set_http_client(unique_ptr<HttpClient> http_client) { m_http_client = move(http_client); }

  /**
   * @brief Set the event store
   * 
   * @param event_store Defines the database to use for event queue
   */
  void set_event_store(shared_ptr<EventStore> event_store);

  /**
   * @brief Set a callback to call after emit requests are made with the resulting emit status.
   * 
   * To subscribe to multiple emit statuses, use binary operations such as `EmitStatus::FAILED_WILL_RETRY | EmitStatus::FAILED_WONT_RETRY`.
   * Calling this function overwrites any previously set callbacks.
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
   * 
   * @param http_status_code HTTP status code
   * @param retry Whether events should be retried or not
   */
  void set_custom_retry_for_status_code(int http_status_code, bool retry);

  /**
   * @brief Get the event store.
   * 
   * If SQLite database path was passed in the constructor, this method initializes a new `SqliteStorage` instance.
   * 
   * @return shared_ptr<EventStore> The database to use for event queue.
   */
  shared_ptr<EventStore> get_event_store() const;

  /**
   * @brief Get the db_name path set in the constructor.
   * 
   * @return string Path to the SQLite database to be used as event store (optional).
   */
  string get_db_name() const { return m_db_name; }

  /**
   * @brief Get the batch size limit
   * 
   * @return int The maximum amount of events to send at a time
   */
  int get_batch_size() const { return m_batch_size; }

  /**
   * @brief Get the byte limit for GET requests
   * 
   * @return int The byte limit when sending a GET request
   */
  int get_byte_limit_get() const { return m_byte_limit_get; }

  /**
   * @brief Get the byte limit for POST requests
   * 
   * @return int The byte limit when sending a POST request
   */
  int get_byte_limit_post() const { return m_byte_limit_post; }

  /**
   * @brief Get the optional Emitter request callback function.
   * 
   * @return EmitterCallback Callback function
   */
  EmitterCallback get_request_callback() const { return m_callback; }

  /**
   * @brief Get the optional request callback emit status.
   * 
   * @return EmitStatus Emit status to trigger the request callback for
   */
  EmitStatus get_request_callback_emit_status() const { return m_callback_emit_status; }

  /**
   * @brief Get the custom retry rule settings for HTTP status codes.
   * 
   * @return map<int, bool> Map of status code –> retry or not boolean.
   */
  map<int, bool> get_custom_retry_for_status_codes() const { return m_custom_retry_for_status_codes; }

private:
  /**
   * @brief Retrieve and take ownership of the configured HTTP client. Can only be called once – by the Emitter.
   * 
   * @return unique_ptr<HttpClient> Unique pointer to the HTTP client.
   */
  unique_ptr<HttpClient> move_http_client() { return m_http_client ? move(m_http_client) : nullptr; }

  void shared_init();

  int m_batch_size;
  int m_byte_limit_post;
  int m_byte_limit_get;
  shared_ptr<EventStore> m_event_store;
  unique_ptr<HttpClient> m_http_client;
  EmitterCallback m_callback;
  EmitStatus m_callback_emit_status;
  map<int, bool> m_custom_retry_for_status_codes;
  string m_db_name;

  friend class Emitter;
};
} // namespace snowplow

#endif
