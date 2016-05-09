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

const int post_wrapper_bytes = 88; // "schema":"iglu:com.snowplowanalytics.snowplow/payload_data/jsonschema/1-0-3","data":[]
const int post_stm_bytes = 22;     // "stm":"1443452851000"

Emitter::Emitter(const string & uri, Strategy strategy, Method method, Protocol protocol, int send_limit, const string & db_name) : m_db(db_name) {

  if (uri == "") {
    throw invalid_argument("FATAL: Emitter URI cannot be empty.");
  }

  this->m_uri = uri;
  this->m_strategy = strategy;
  this->m_method = method;
  this->m_protocol = protocol;
  this->m_send_limit = send_limit;

  this->m_url = this->get_collector_url();
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
  unique_lock<mutex> locker(this->m_db_insert);
  this->m_db.insert_payload(payload);
  this->m_check_db.notify_all();
  locker.unlock();
}

void Emitter::flush() {
  this->m_check_db.notify_all();
}

// --- Private

void Emitter::run() {
  list<Storage::EventRow>* event_rows = new list<Storage::EventRow>;
  list<HttpRequestResult>* results = new list<HttpRequestResult>;
  list<int>* success_ids = new list<int>;

  do {
    unique_lock<mutex> locker(this->m_db_select);
    this->m_check_db.wait(locker);
    this->m_db.select_event_row_range(event_rows, this->m_send_limit);
    locker.unlock();

    if (event_rows->size() > 0) {
      this->do_send(event_rows, results);

      // Process results
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
      this->m_db.delete_event_row_ids(success_ids);

      // TODO: Add callback function
      cout << "Success: " << success_count << endl;
      cout << "Failure: " << failure_count << endl;

      // Reset collections
      event_rows->clear();
      results->clear();
      success_ids->clear();
    }
  } while (is_running());

  delete(event_rows);
  delete(results);
  delete(success_ids);
}

void Emitter::do_send(list<Storage::EventRow>* event_rows, list<HttpRequestResult>* results) {
  list<std::future<HttpRequestResult>> request_futures;

  // Send each request in its own thread
  if (this->m_method == GET) {
    for (list<Storage::EventRow>::iterator it = event_rows->begin(); it != event_rows->end(); ++it) {
      map<string, string> event_map = it->event.get();
      event_map["stm"] = std::to_string(Utils::get_unix_epoch_ms());
      string final_url = this->m_url + "?" + Utils::map_to_query_string(event_map);
      list<int> row_ids = {it->id};

      // TODO: Add checking of byte size
      request_futures.push_back(std::async(HttpClient::http_get, final_url, row_ids, false));
    }
  }

  // Grab all the request results and return
  for (list<std::future<HttpRequestResult>>::iterator it = request_futures.begin(); it != request_futures.end(); ++it) {
    results->push_back(it->get());
  }
  request_futures.clear();
}

// --- Helpers

string Emitter::get_collector_url() {
  stringstream url;
  url << (this->m_protocol == HTTP ? "http" : "https") << "://" << this->m_uri;
  url << (this->m_method == GET ? "/i" : "/com.snowplowanalytics.snowplow/tp2");
  return url.str();
}

bool Emitter::is_running() {
  lock_guard<mutex> guard(this->m_run_check);
  return this->m_running;
}
