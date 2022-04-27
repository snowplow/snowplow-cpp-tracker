#include <iostream>
#include <string>
#include <time.h>

#include "../src/snowplow.hpp"

using snowplow::NetworkConfiguration;
using snowplow::EmitterConfiguration;
using snowplow::TrackerConfiguration;
using snowplow::SessionConfiguration;
using snowplow::EmitStatus;
using snowplow::Subject;
using snowplow::StructuredEvent;
using snowplow::ScreenViewEvent;
using snowplow::TimingEvent;
using snowplow::SqliteStorage;
using snowplow::Method;
using snowplow::Snowplow;
using std::cout;
using std::endl;
using std::string;
using std::make_shared;

void usage(char *program_name) {
  cout << "Usage: " << program_name << " [COLLECTOR_URI]" << endl;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    usage(argv[0]);
    return 1;
  }

  // Create Tracker Variables
  string uri = argv[1];
  string db_name = "demo.db";

  EmitterConfiguration emitter_config(db_name);
  emitter_config.set_batch_size(500);
  emitter_config.set_byte_limit_get(5200);
  emitter_config.set_byte_limit_post(5200);
  emitter_config.set_request_callback(
      [](list<string> event_ids, EmitStatus emit_status) {
        switch (emit_status) {
        case EmitStatus::SUCCESS:
          printf("Successfuly sent %lu events.\n", event_ids.size());
          break;
        case EmitStatus::FAILED_WILL_RETRY:
          printf("Failed to send %lu events, but will retry.\n", event_ids.size());
          break;
        case EmitStatus::FAILED_WONT_RETRY:
          printf("Failed to send %lu events and won't retry.\n", event_ids.size());
          break;
        }
      },
      EmitStatus::SUCCESS | EmitStatus::FAILED_WILL_RETRY | EmitStatus::FAILED_WONT_RETRY);

  auto subject = make_shared<Subject>();
  subject->set_user_id("a-user-id");
  subject->set_screen_resolution(1920, 1080);
  subject->set_viewport(1080, 1080);
  subject->set_color_depth(32);
  subject->set_timezone("GMT");
  subject->set_language("EN");
  subject->set_useragent("Mozilla/5.0");

  TrackerConfiguration tracker_config("namespace", "app-id", "mob");
  tracker_config.set_use_base64(false);
  tracker_config.set_desktop_context(true);

  SessionConfiguration session_config(db_name, 5000, 5000);

  // Create Tracker
  auto tracker = Snowplow::create_tracker(
      tracker_config,
      NetworkConfiguration(uri, Method::POST),
      emitter_config,
      session_config,
      subject);

  time_t start, end;
  time(&start);

  for (int i = 0; i < 2000; i++) {
    TimingEvent te("timing-cat", "timing-var", 123);

    ScreenViewEvent sve;
    string name = "Screen ID - 5asd56";
    sve.name = &name;

    StructuredEvent se("shop", "add-to-basket");
    string property = "pcs";
    double value = 25.6;
    se.property = &property;
    se.value = &value;

    tracker->track(te);
    tracker->track(sve);
    tracker->track(se);
  }

  time(&end);
  double diff = difftime(end, start);
  printf("It took me %f seconds to build and store 6000 events.\n", diff);

  // Flush and close
  tracker->flush();

  time(&end);
  diff = difftime(end, start);
  printf("It took me %f seconds to send 6000 events.\n", diff);

  return 0;
}
