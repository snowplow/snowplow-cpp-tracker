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

#ifndef EMITTER_H
#define EMITTER_H

#include <string>
#include <sstream>
#include <condition_variable>
#include <future>
#include <thread>
#include "constants.hpp"
#include "utils.hpp"
#include "storage.hpp"
#include "payload.hpp"
#include "self_describing_json.hpp"
#include "http_client.hpp"
#include "http_request_result.hpp"
#include "cracked_url.hpp"

using namespace std;

class Emitter {
public:
  enum Method {
    POST,
    GET
  };

  enum Protocol {
    HTTP,
    HTTPS
  };

  Emitter(const string & uri, Method method, Protocol protocol, int send_limit, 
    int byte_limit_post, int byte_limit_get, const string & db_name);
  ~Emitter();
  virtual void start();
  virtual void stop();
  virtual void add(Payload payload);
  virtual void flush();

private:
  CrackedUrl m_url;
  Method m_method;
  unsigned int m_send_limit;
  unsigned int m_byte_limit_get;
  unsigned int m_byte_limit_post;
  string m_db_name;

  thread m_daemon_thread;
  condition_variable m_check_db;
  condition_variable m_check_fin;
  mutex m_flush_fin;
  mutex m_db_select;
  mutex m_run_check;
  bool m_running;

  void run();
  void do_send(list<Storage::EventRow>* event_rows, list<HttpRequestResult>* results);
  string build_post_data_json(list<Payload> payload_list);
  string get_collector_url(const string & uri, Protocol protocol, Method method);
  bool is_running();
};

#endif
