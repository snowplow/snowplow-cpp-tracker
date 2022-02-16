#include <fstream>
#include <string>

#include "run.hpp"
#include "../src/utils.hpp"
#include "../src/storage.hpp"

using namespace std;

int main(int argc, char **argv) {
  // run and measure performance
  string db_name = "performance.db";
  double mocked_emitter_and_mocked_session = run_mocked_emitter_and_mocked_session(db_name);
  double mocked_emitter_and_real_session = run_mocked_emitter_and_real_session(db_name);
  double mute_emitter_and_mocked_session = run_mute_emitter_and_mocked_session(db_name);
  double mute_emitter_and_real_session = run_mute_emitter_and_real_session(db_name);

  // print results
  cout << endl << "RESULTS (" << NUM_OPERATIONS << " operations x " << NUM_THREADS << " threads)" << endl << endl;
  cout << "Mocked emitter and mocked session: " << mocked_emitter_and_mocked_session << " seconds" << endl;
  cout << "Mocked emitter and real session: " << mocked_emitter_and_real_session << " seconds" << endl;
  cout << "Mute emitter and mocked session: " << mute_emitter_and_mocked_session << " seconds" << endl;
  cout << "Mute emitter and real session: " << mute_emitter_and_real_session << " seconds" << endl;

  // store results in logs as JSON
  json results;
  results["num_operations"] = NUM_OPERATIONS;
  results["num_threads"] = NUM_THREADS;
  results["mocked_emitter_and_mocked_session"] = mocked_emitter_and_mocked_session;
  results["mocked_emitter_and_real_session"] = mocked_emitter_and_real_session;
  results["mute_emitter_and_mocked_session"] = mute_emitter_and_mocked_session;
  results["mute_emitter_and_real_session"] = mute_emitter_and_real_session;

  SelfDescribingJson desktop_context = Utils::get_desktop_context();
  json desktop_context_json = desktop_context.get();
  json output;
  output["desktop_context"] = desktop_context_json;
  output["results"] = results;
  output["timestamp"] = Utils::get_unix_epoch_ms();
  output["tracker_version"] = SNOWPLOW_TRACKER_VERSION_LABEL;

  ofstream outfile;
  outfile.open("performance/logs.txt", std::ios_base::app);
  outfile << output.dump() << endl; 
  outfile.close();

  return 0;
}
