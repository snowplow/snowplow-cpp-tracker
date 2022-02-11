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
