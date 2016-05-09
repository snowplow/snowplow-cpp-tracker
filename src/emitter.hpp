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
#include "storage.hpp"
#include "payload.hpp"
#include "http_client.hpp"
#include "http_request_result.hpp"

using namespace std;

class Emitter {
public:
  enum Strategy {
    SYNC,
    ASYNC
  };

  enum Method {
    POST,
    GET
  };

  enum Protocol {
    HTTP,
    HTTPS
  };

  Emitter(const string & uri, Strategy strategy, Method method, Protocol protocol, int send_limit, const string & db_name);
  void add(Payload payload);
  void stop();
  void start();
  void flush();

private:
  string m_uri;
  string m_url;
  Strategy m_strategy;
  Method m_method;
  Protocol m_protocol;
  int m_send_limit;
  Storage m_db;

  thread m_daemon_thread;
  condition_variable m_check_db;
  mutex m_db_access;
  mutex m_run_check; 
  bool m_running;
  bool is_running();
  void run();
  string get_collector_url();
  void do_send(list<Storage::EventRow>* event_rows, list<HttpRequestResult>* results);
};

#endif
