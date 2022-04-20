#include <iostream>
#include <string>
#include <time.h>

#include "../src/snowplow.hpp"

using snowplow::ClientSession;
using snowplow::Emitter;
using snowplow::EmitStatus;
using snowplow::Subject;
using snowplow::Tracker;
using snowplow::StructuredEvent;
using snowplow::ScreenViewEvent;
using snowplow::TimingEvent;
using snowplow::SqliteStorage;
using std::cout;
using std::endl;
using std::string;

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

  auto storage = std::make_shared<SqliteStorage>(db_name);
  Emitter emitter(uri, Emitter::Method::POST, Emitter::Protocol::HTTP, 500, 52000, 52000, storage);
  emitter.set_request_callback(
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

  Subject subject;
  subject.set_user_id("a-user-id");
  subject.set_screen_resolution(1920, 1080);
  subject.set_viewport(1080, 1080);
  subject.set_color_depth(32);
  subject.set_timezone("GMT");
  subject.set_language("EN");
  subject.set_useragent("Mozilla/5.0");

  ClientSession client_session(storage, 5000, 5000);

  string platform = "mob";
  string app_id = "app-id";
  string name_space = "namespace";
  bool base64 = false;
  bool desktop_context = true;

  // Create Tracker
  Tracker *t = Tracker::init(emitter, &subject, &client_session, &platform, &app_id, &name_space, &base64, &desktop_context);

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

    t->track(te);
    t->track(sve);
    t->track(se);
  }

  time(&end);
  double diff = difftime(end, start);
  printf("It took me %f seconds to build and store 6000 events.\n", diff);

  // Flush and close
  t->flush();
  Tracker::close();

  time(&end);
  diff = difftime(end, start);
  printf("It took me %f seconds to send 6000 events.\n", diff);

  return 0;
}
