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
  this->m_db.insert_payload(payload);
}

void Emitter::start() {
  list<Storage::EventRow>* event_rows = new list<Storage::EventRow>;
  this->m_db.select_event_row_range(event_rows, this->m_send_limit);

  // Exit if no events are found
  if (event_rows->size() == 0) {
    delete(event_rows);
    return;
  }

  list<HttpRequestResult>* results = new list<HttpRequestResult>;
  this->do_send(event_rows, results);

  // Return memory
  event_rows->clear();
  results->clear();
  delete(event_rows);
  delete(results);
}

void Emitter::do_send(list<Storage::EventRow>* event_rows, list<HttpRequestResult>* results) {
  if (this->m_method == GET) {
    for (list<Storage::EventRow>::iterator it = event_rows->begin(); it != event_rows->end(); ++it) {
      string final_url = m_url + "?" + Utils::map_to_query_string(it->event.get());
      results->push_back(HttpClient::http_get(final_url));
    }
  }
}
