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

#include "emitter.hpp"

using namespace snowplow;
using std::invalid_argument;
using std::lock_guard;
using std::stringstream;
using std::unique_lock;
using std::shared_ptr;
using std::unique_ptr;
using std::async;
using std::to_string;
using std::transform;
using std::equal;
using std::future;
using std::this_thread::sleep_for;

const int post_wrapper_bytes = 88; // "schema":"iglu:com.snowplowanalytics.snowplow/payload_data/jsonschema/1-0-4","data":[]
const int post_stm_bytes = 22;     // "stm":"1443452851000"

#if defined(__APPLE__)
#include "../http/http_client_apple.hpp"
unique_ptr<HttpClient> createDefaultHttpClient(const string &curl_cookie_file) {
  return unique_ptr<HttpClient>(new HttpClientApple());
}
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include "../http/http_client_windows.hpp"
unique_ptr<HttpClient> createDefaultHttpClient(const string &curl_cookie_file) {
  return unique_ptr<HttpClient>(new HttpClientWindows());
}
#else
#include "../http/http_client_curl.hpp"
unique_ptr<HttpClient> createDefaultHttpClient(const string &curl_cookie_file) {
  return unique_ptr<HttpClient>(new HttpClientCurl(curl_cookie_file));
}
#endif

Emitter::Emitter(NetworkConfiguration &network_config, const EmitterConfiguration &emitter_config) :
  Emitter(
    emitter_config.get_event_store(),
    network_config.get_collector_hostname(),
    network_config.get_method(),
    network_config.get_protocol(),
    emitter_config.get_batch_size(),
    emitter_config.get_byte_limit_post(),
    emitter_config.get_byte_limit_get(),
    network_config.move_http_client(),
    network_config.get_curl_cookie_file()
  ) {
  m_callback = emitter_config.get_request_callback();
  m_callback_emit_status = emitter_config.get_request_callback_emit_status();
  m_custom_retry_for_status_codes = emitter_config.get_custom_retry_for_status_codes();
}

Emitter::Emitter(shared_ptr<EventStore> event_store, const string &uri, Method method, Protocol protocol, int batch_size,
                 int byte_limit_post, int byte_limit_get,
                 unique_ptr<HttpClient> http_client, const string &curl_cookie_file) : m_url(this->get_collector_url(uri, protocol, method)) {
  if (uri == "") {
    throw invalid_argument("FATAL: Emitter URI cannot be empty.");
  }

  string expected_http = "http://";
  string expected_https = "https://";
  string actual_uri_lower = uri;
  transform(actual_uri_lower.begin(), actual_uri_lower.end(), actual_uri_lower.begin(), ::tolower);

  if ((expected_http.size() <= actual_uri_lower.size() && equal(expected_http.begin(), expected_http.end(), actual_uri_lower.begin())) ||
      (expected_https.size() <= actual_uri_lower.size() && equal(expected_https.begin(), expected_https.end(), actual_uri_lower.begin()))) {
    throw invalid_argument("FATAL: Emitter URI (" + uri + ") must not start with http:// or https://");
  }

  if (!this->m_url.get_is_valid()) {
    throw invalid_argument("FATAL: Emitter URL is not valid - " + this->m_url.to_string());
  }

  this->m_running = false;
  this->m_method = method;
  this->m_batch_size = batch_size;
  this->m_byte_limit_post = byte_limit_post;
  this->m_byte_limit_get = byte_limit_get;
  this->m_event_store = std::move(event_store);
  if (http_client) {
    this->m_http_client = std::move(http_client);
  } else {
    this->m_http_client = createDefaultHttpClient(curl_cookie_file);
  }
}

Emitter::~Emitter() {
  this->stop();
}

// --- Controls

void Emitter::start() {
  unique_lock<mutex> locker(this->m_run_check);
  if (this->m_running) {
    return; // refuse to start more than once
  }
  this->m_running = true;
  this->m_daemon_thread = thread(&Emitter::run, this);
}

void Emitter::stop() {
  unique_lock<mutex> locker(this->m_run_check);
  if (this->m_running == true) {
    this->m_running = false;
    locker.unlock();

    this->m_check_db.notify_all();
    this->m_daemon_thread.join();
  }
}

void Emitter::add(Payload payload) {
  m_event_store->add_event(payload);
  this->m_check_db.notify_all();
}

void Emitter::flush() {
  unique_lock<mutex> locker_1(this->m_run_check);
  if (this->m_running == false) {
    return;
  }
  locker_1.unlock();

  this->m_check_db.notify_all();

  unique_lock<mutex> locker_2(this->m_flush_fin);
  this->m_check_fin.wait(locker_2);
  locker_2.unlock();

  this->stop();
}

// --- Private

void Emitter::run() {
  do {
    list<EventRow> event_rows;
    m_event_store->get_event_rows_batch(&event_rows, m_batch_size);

    if (event_rows.size() > 0) {
      // emit the events
      list<HttpRequestResult> results;
      do_send(event_rows, &results);

      // classify results into successful and failed
      list<int> success_row_ids;
      list<int> failed_will_retry_row_ids;
      list<int> failed_wont_retry_row_ids;
      for (auto const &result : results) {
        auto res_row_ids = result.get_row_ids();
        if (result.is_success()) {
          success_row_ids.splice(success_row_ids.end(), res_row_ids);
        } else if (result.should_retry(m_custom_retry_for_status_codes)) {
          failed_will_retry_row_ids.splice(failed_will_retry_row_ids.end(), res_row_ids);
        } else {
          failed_wont_retry_row_ids.splice(failed_wont_retry_row_ids.end(), res_row_ids);
        }
      }

      // trigger callbacks if enabled
      trigger_callbacks(success_row_ids, failed_will_retry_row_ids, failed_wont_retry_row_ids, event_rows);

      // delete rows with successfully sent events and failed events that should not be retried
      list<int> delete_row_ids;
      delete_row_ids.splice(delete_row_ids.end(), success_row_ids);
      delete_row_ids.splice(delete_row_ids.end(), failed_wont_retry_row_ids);
      m_event_store->delete_event_rows_with_ids(delete_row_ids);

      // update retry delay calculation based on whether the requests will be retried
      if (!failed_will_retry_row_ids.empty()) {
        m_retry_delay.will_retry_emit();
      } else {
        m_retry_delay.wont_retry_emit();
      }

      // sleep for the retry delay if there is one
      auto retry_delay = m_retry_delay.get();
      if (retry_delay.count() > 0) {
        sleep_for(retry_delay);
      }
    } else {
      m_check_fin.notify_all();

      // if there are no events to send, sleep for a while
      unique_lock<mutex> locker(m_db_select);
      m_check_db.wait_for(locker, std::chrono::seconds(5));
    }
  } while (is_running());
}

void Emitter::do_send(const list<EventRow> &event_rows, list<HttpRequestResult> *results) {
  list<future<HttpRequestResult>> request_futures;

  // Send each request in its own thread
  if (this->m_method == GET) {
    for (auto const &row : event_rows) {
      Payload event_payload = row.event;
      event_payload.add(SNOWPLOW_SENT_TIMESTAMP, to_string(Utils::get_unix_epoch_ms()));
      string query_string = Utils::map_to_query_string(event_payload.get());
      list<int> row_id = {row.id};

      request_futures.push_back(async(&HttpClient::http_get, this->m_http_client.get(), this->m_url, query_string, row_id, (query_string.size() > this->m_byte_limit_get)));
    }
  } else {
    list<int> row_ids;
    list<Payload> payloads;
    int total_byte_size = 0;

    for (auto const &row : event_rows) {
      unsigned int byte_size = unsigned(Utils::serialize_payload(row.event).size() + post_stm_bytes);

      if ((byte_size + post_wrapper_bytes) > this->m_byte_limit_post) {
        // A single payload has exceeded the Byte Limit
        list<int> single_row_id = {row.id};
        list<Payload> single_payload = {row.event};
        request_futures.push_back(async(&HttpClient::http_post, this->m_http_client.get(), this->m_url, this->build_post_data_json(single_payload), single_row_id, true));

        single_row_id.clear();
        single_payload.clear();
      } else if ((total_byte_size + byte_size + post_wrapper_bytes + (payloads.size() - 1)) > this->m_byte_limit_post) {
        // Byte limit reached
        request_futures.push_back(async(&HttpClient::http_post, this->m_http_client.get(), this->m_url, this->build_post_data_json(payloads), row_ids, false));

        // Reset accumulators
        row_ids.clear();
        row_ids = {row.id};
        payloads.clear();
        payloads = {row.event};
        total_byte_size = byte_size;
      } else {
        row_ids.push_back(row.id);
        payloads.push_back(row.event);
        total_byte_size += byte_size;
      }
    }

    if (payloads.size() > 0) {
      request_futures.push_back(async(&HttpClient::http_post, this->m_http_client.get(), this->m_url, this->build_post_data_json(payloads), row_ids, false));
    }
  }

  // Grab all the request results and return
  for (auto it = request_futures.begin(); it != request_futures.end(); ++it) {
    results->push_back(it->get());
  }
  request_futures.clear();
}

void Emitter::trigger_callbacks(const list<int> &success_row_ids, const list<int> &failed_will_retry_row_ids, const list<int> &failed_wont_retry_row_ids, const list<EventRow> &event_rows) const {
  if (!m_callback) {
    return;
  }

  // create a mapping table and function between row IDs and event IDs
  map<int, string> event_ids_for_row_ids;
  for (auto const &row : event_rows) {
    auto payload = row.event.get();
    auto it = payload.find(SNOWPLOW_EID);
    if (it != payload.end()) {
      event_ids_for_row_ids.insert({row.id, it->second});
    }
  }
  auto transform_row_ids_to_event_ids = [&](const list<int> &row_ids) {
    list<string> event_ids;
    for (auto const &row_id : row_ids) {
      auto it = event_ids_for_row_ids.find(row_id);
      if (it != event_ids_for_row_ids.end()) {
        event_ids.push_back(it->second);
      }
    }
    return event_ids;
  };

  // execute callback for successful events
  if ((m_callback_emit_status & SUCCESS) && !success_row_ids.empty()) {
    list<string> success_event_ids = transform_row_ids_to_event_ids(success_row_ids);
    execute_callback(success_event_ids, SUCCESS);
  }

  // execute callback for failed events that will be retried
  if ((m_callback_emit_status & FAILED_WILL_RETRY) && !failed_will_retry_row_ids.empty()) {
    list<string> failed_will_retry_event_ids = transform_row_ids_to_event_ids(failed_will_retry_row_ids);
    execute_callback(failed_will_retry_event_ids, FAILED_WILL_RETRY);
  }

  // execute callback for failed events that won't be retried
  if ((m_callback_emit_status & FAILED_WONT_RETRY) && !failed_wont_retry_row_ids.empty()) {
    list<string> failed_wont_retry_event_ids = transform_row_ids_to_event_ids(failed_wont_retry_row_ids);
    execute_callback(failed_wont_retry_event_ids, FAILED_WONT_RETRY);
  }
}

void Emitter::execute_callback(const list<string> &event_ids, EmitStatus emit_status) const {
  thread(m_callback, event_ids, emit_status).detach();
}

// --- Helpers

string Emitter::build_post_data_json(list<Payload> payload_list) {
  json data_array = json::array();

  // Add 'stm' to each payload
  string stm = to_string(Utils::get_unix_epoch_ms());
  for (list<Payload>::iterator it = payload_list.begin(); it != payload_list.end(); ++it) {
    it->add(SNOWPLOW_SENT_TIMESTAMP, stm);
    data_array.push_back(it->get());
  }

  // Build Post event
  SelfDescribingJson post_envelope(SNOWPLOW_SCHEMA_PAYLOAD_DATA, data_array);
  return post_envelope.to_string();
}

string Emitter::get_collector_url(const string &uri, Protocol protocol, Method method) const {
  stringstream url;
  url << (protocol == HTTP ? "http" : "https") << "://" << uri;
  url << "/" << (method == GET ? SNOWPLOW_GET_PROTOCOL_PATH : SNOWPLOW_POST_PROTOCOL_VENDOR + "/" + SNOWPLOW_POST_PROTOCOL_VERSION);
  return url.str();
}

bool Emitter::is_running() {
  lock_guard<mutex> guard(this->m_run_check);
  return this->m_running;
}

void Emitter::set_request_callback(const EmitterCallback &callback, EmitStatus emit_status) {
  lock_guard<mutex> guard(this->m_run_check);
  if (m_running) {
    throw std::logic_error("Not allowed when Emitter is running");
  }

  m_callback_emit_status = emit_status;
  m_callback = callback;
}

void Emitter::set_custom_retry_for_status_code(int http_status_code, bool retry) {
  lock_guard<mutex> guard(this->m_run_check);
  if (m_running) {
    throw std::logic_error("Not allowed when Emitter is running");
  }
  if (http_status_code < 300) {
    throw std::invalid_argument("Retry rules can only be set for status codes >= 300");
  }

  m_custom_retry_for_status_codes.insert({http_status_code, retry});
}
