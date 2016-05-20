#include <iostream>
#include <string>

#include "../src/tracker.hpp"

using namespace std;

int main(int argc, char** argv) {
  if (argc != 2) {
    throw invalid_argument("Requires 2 argument got " + to_string(argc));
  }

  // Create Tracker Variables
  string url = argv[1];
  string db_name = "demo.db";

  Emitter emitter(url, Emitter::Method::POST, Emitter::Protocol::HTTP, 52000, 52000, 500, db_name);

  Subject subject;
  subject.set_user_id("a-user-id");
  subject.set_screen_resolution(1920, 1080);
  subject.set_viewport(1080, 1080);
  subject.set_color_depth(32);
  subject.set_timezone("GMT");
  subject.set_language("EN");

  ClientSession client_session(db_name, 5000, 5000, 2500);

  string platform = "mob";
  string app_id = "app-id";
  string name_space = "namespace";
  bool base64 = false;

  // Create Tracker
  Tracker *t = Tracker::init(emitter, &subject, &client_session, &platform, &app_id, &name_space, &base64);

  // Add some custom contexts
  vector<SelfDescribingJson> contexts;
  SelfDescribingJson custom_context("iglu:com.acme/some_event/jsonschema/1-0-0", "{\"event\":\"data\"}"_json);
  contexts.push_back(custom_context);

  // Track some events
  Tracker::TimingEvent timing_event("timing-cat", "timing-var", 123);
  timing_event.contexts = contexts;

  for (int i = 0; i < 100; i++) {
    t->track_timing(timing_event);
  }

  // Flush and close
  t->flush();
  Tracker::close();
  return 0;
}
