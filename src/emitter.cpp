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

// --- Helpers

string Emitter::get_collector_url() {
  stringstream url;
  url << (this->m_protocol == HTTP ? "http" : "https") << "://" << this->m_uri;
  url << (this->m_method == GET ? "/i" : "/com.snowplowanalytics.snowplow/tp2");
  return url.str();
}

// --- Processors

void Emitter::add(Payload payload) {
  unique_lock<mutex> locker(this->m_db_access);
  this->m_db.insert_payload(payload);
  locker.unlock();
  this->m_check_db.notify_all();
}

void Emitter::run() {
  do {
    unique_lock<mutex> locker(this->m_db_access);
    this->m_check_db.wait(locker);

    list<Storage::EventRow>* event_rows = new list<Storage::EventRow>;
    this->m_db.select_event_row_range(event_rows, this->m_send_limit);

    locker.unlock();

    // Wait if no events are found
    if (event_rows->size() != 0) {
      list<HttpRequestResult>* results = new list<HttpRequestResult>;
      this->do_send(event_rows, results);

      // Process results
      int success_count = 0;
      int failure_count = 0;
      list<int>* success_ids = new list<int>;

      for (list<HttpRequestResult>::iterator it = results->begin(); it != results->end(); ++it) {
        
      }

      // Return memory
      event_rows->clear();
      results->clear();
      delete(event_rows);
      delete(results);
    } else {
      delete(event_rows);
    }
  } while (is_running());
}

void Emitter::start() {
  unique_lock<mutex> set_running(this->m_run_check);
  if (this->m_running) {
    set_running.unlock(); // refuse to start more than once
    return;
  }
  this->m_running = true;
  this->m_daemon_thread = thread(&Emitter::run, this);
  set_running.unlock();
}

bool Emitter::is_running() {
  lock_guard<mutex> guard(this->m_run_check);
  return this->m_running;
}

void Emitter::stop() {
  unique_lock<mutex> running_lock(this->m_run_check);

  if (this->m_running == true) {
    this->m_running = false;
    running_lock.unlock();
    flush();
    this->m_daemon_thread.join();
  }
  else {
    running_lock.unlock();
  }
}

void Emitter::flush() {
  this->m_check_db.notify_all();
}

void Emitter::do_send(list<Storage::EventRow>* event_rows, list<HttpRequestResult>* results) {
  list<std::future<HttpRequestResult>>* request_futures = new list<std::future<HttpRequestResult>>;

  // Send each request in its own thread
  if (this->m_method == GET) {
    for (list<Storage::EventRow>::iterator it = event_rows->begin(); it != event_rows->end(); ++it) {
      map<string, string> event_map = it->event.get();
      event_map["stm"] = std::to_string(Utils::get_unix_epoch_ms());

      string final_url = this->m_url + "?" + Utils::map_to_query_string(event_map);

      std::packaged_task<HttpRequestResult(string)> task([](string final_url) { return HttpClient::http_get(final_url); });
      request_futures->push_back(task.get_future());
      std::thread(std::move(task), final_url).detach();
    }
  }

  // Grab all the request results and return
  for (list<std::future<HttpRequestResult>>::iterator it = request_futures->begin(); it != request_futures->end(); ++it) {
    results->push_back(it->get());
  }
  request_futures->clear();
  delete(request_futures);
}
