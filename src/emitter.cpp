/*
Copyright (c) 2016 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#include "emitter.hpp"

const int post_wrapper_bytes = 88; // "schema":"iglu:com.snowplowanalytics.snowplow/payload_data/jsonschema/1-0-4","data":[]
const int post_stm_bytes = 22;     // "stm":"1443452851000"

Emitter::Emitter(const string & uri, Method method, Protocol protocol, int send_limit, 
  int byte_limit_post, int byte_limit_get, const string & db_name) : m_url(this->get_collector_url(uri, protocol, method)) {

  if (uri == "") {
    throw invalid_argument("FATAL: Emitter URI cannot be empty.");
  }

  if (!this->m_url.get_is_valid()) {
    throw invalid_argument("FATAL: Emitter URL is not valid - " + this->m_url.to_string());
  }

  this->m_running = false;
  this->m_method = method;
  this->m_send_limit = send_limit;
  this->m_byte_limit_post = byte_limit_post;
  this->m_byte_limit_get = byte_limit_get;
  this->m_db_name = db_name;
}

Emitter::~Emitter() {
  this->stop();
}

// --- Controls

void Emitter::start() {
  unique_lock<mutex> locker(this->m_run_check);
  if (this->m_running) {
    locker.unlock(); // refuse to start more than once
    return;
  }
  this->m_running = true;
  this->m_daemon_thread = thread(&Emitter::run, this);
  locker.unlock();
}

void Emitter::stop() {
  unique_lock<mutex> locker(this->m_run_check);
  if (this->m_running == true) {
    this->m_running = false;
    locker.unlock();

    this->m_check_db.notify_all();
    this->m_daemon_thread.join();
  } else {
    locker.unlock();
  }
}

void Emitter::add(Payload payload) {
  Storage::instance(this->m_db_name)->insert_payload(payload);
  this->m_check_db.notify_all();
}

void Emitter::flush() {
  unique_lock<mutex> locker_1(this->m_run_check);
  if (this->m_running == false) {
    locker_1.unlock();
    return;
  }

  this->m_check_db.notify_all();

  unique_lock<mutex> locker_2(this->m_flush_fin);
  this->m_check_fin.wait(locker_2);
  locker_2.unlock();

  this->stop();
}

// --- Private

void Emitter::run() {
  do {
    list<Storage::EventRow>* event_rows = new list<Storage::EventRow>;
    Storage::instance(this->m_db_name)->select_event_row_range(event_rows, this->m_send_limit);

    if (event_rows->size() > 0) {
      list<HttpRequestResult>* results = new list<HttpRequestResult>;
      this->do_send(event_rows, results);

      // Process results
      list<int>* success_ids = new list<int>;
      int success_count = 0;
      int failure_count = 0;
      
      for (list<HttpRequestResult>::iterator it = results->begin(); it != results->end(); ++it) {
        list<int> res_row_ids = it->get_row_ids();
        if (it->is_success()) {
          success_ids->splice(success_ids->end(), res_row_ids);
        } else {
          failure_count += res_row_ids.size();
        } 
      }
      success_count = success_ids->size();
      Storage::instance(this->m_db_name)->delete_event_row_ids(success_ids);

      // TODO: Add callback function
      cout << "Success: " << success_count << endl;
      cout << "Failure: " << failure_count << endl;

      // Reset collections
      event_rows->clear();
      results->clear();
      success_ids->clear();
      delete(event_rows);
      delete(results);
      delete(success_ids);
    } else {
      delete(event_rows);

      this->m_check_fin.notify_all();

      unique_lock<mutex> locker(this->m_db_select);
      this->m_check_db.wait(locker);
      locker.unlock();
    }
  } while (this->is_running());
}

void Emitter::do_send(list<Storage::EventRow>* event_rows, list<HttpRequestResult>* results) {
  list<std::future<HttpRequestResult>> request_futures;

  // Send each request in its own thread
  if (this->m_method == GET) {
    for (list<Storage::EventRow>::iterator it = event_rows->begin(); it != event_rows->end(); ++it) {
      Payload event_payload = it->event;
      event_payload.add(SNOWPLOW_SENT_TIMESTAMP, std::to_string(Utils::get_unix_epoch_ms()));
      string query_string = Utils::map_to_query_string(event_payload.get());
      list<int> row_id = {it->id};

      request_futures.push_back(std::async(HttpClient::http_get, this->m_url, query_string, row_id, (query_string.size() > this->m_byte_limit_get)));
    }
  } else {
    list<int> row_ids;
    list<Payload> payloads;
    int total_byte_size = 0;

    for (list<Storage::EventRow>::iterator it = event_rows->begin(); it != event_rows->end(); ++it) {
      unsigned int byte_size = Utils::serialize_payload(it->event).size() + post_stm_bytes;

      if ((byte_size + post_wrapper_bytes) > this->m_byte_limit_post) {
        // A single payload has exceeded the Byte Limit
        list<int> single_row_id = {it->id};
        list<Payload> single_payload = {it->event};
        request_futures.push_back(std::async(HttpClient::http_post, this->m_url, this->build_post_data_json(single_payload), single_row_id, true));

        single_row_id.clear();
        single_payload.clear();
      } else if ((total_byte_size + byte_size + post_wrapper_bytes + (payloads.size() -1)) > this->m_byte_limit_post) {
        // Byte limit reached
        request_futures.push_back(std::async(HttpClient::http_post, this->m_url, this->build_post_data_json(payloads), row_ids, false));

        // Reset accumulators
        row_ids.clear();
        row_ids = {it->id};
        payloads.clear();
        payloads = {it->event};
        total_byte_size = byte_size;
      } else {
        row_ids.push_back(it->id);
        payloads.push_back(it->event);
        total_byte_size += byte_size;
      }
    }

    if (payloads.size() > 0) {
      request_futures.push_back(std::async(HttpClient::http_post, this->m_url, this->build_post_data_json(payloads), row_ids, false));
    }
  }

  // Grab all the request results and return
  for (list<std::future<HttpRequestResult>>::iterator it = request_futures.begin(); it != request_futures.end(); ++it) {
    results->push_back(it->get());
  }
  request_futures.clear();
}

// --- Helpers

string Emitter::build_post_data_json(list<Payload> payload_list) {
  json data_array = json::array();

  // Add 'stm' to each payload
  string stm = std::to_string(Utils::get_unix_epoch_ms());
  for (list<Payload>::iterator it = payload_list.begin(); it != payload_list.end(); ++it) {
    it->add(SNOWPLOW_SENT_TIMESTAMP, stm);
    data_array.push_back(it->get());
  }

  // Build Post event
  SelfDescribingJson post_envelope(SNOWPLOW_SCHEMA_PAYLOAD_DATA, data_array);
  return post_envelope.to_string();
}

string Emitter::get_collector_url(const string & uri, Protocol protocol, Method method) {
  stringstream url;
  url << (protocol == HTTP ? "http" : "https") << "://" << uri;
  url << "/" << (method == GET ? SNOWPLOW_GET_PROTOCOL_PATH : SNOWPLOW_POST_PROTOCOL_VENDOR + "/" + SNOWPLOW_POST_PROTOCOL_VERSION);
  return url.str();
}

bool Emitter::is_running() {
  lock_guard<mutex> guard(this->m_run_check);
  return this->m_running;
}
