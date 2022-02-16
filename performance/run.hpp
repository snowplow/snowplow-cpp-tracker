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

#ifndef RUN_H
#define RUN_H

#include <string>

using namespace std;

const int NUM_OPERATIONS = 10000;
const int NUM_THREADS = 5;

double run_mocked_emitter_and_mocked_session(const string & db_name);
double run_mocked_emitter_and_real_session(const string & db_name);
double run_mute_emitter_and_mocked_session(const string & db_name);
double run_mute_emitter_and_real_session(const string & db_name);

#endif
