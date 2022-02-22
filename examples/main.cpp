#include <iostream>
#include <string>
#include <time.h>

#include "../src/tracker.hpp"

using std::string;
using std::cout;
using std::endl;

void usage(char *program_name) {
  cout << "Usage: " << program_name << " [COLLECTOR_URI]" << endl;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    usage(argv[0]);
    return 1;
  }

  // Create Tracker Variables
  string uri = argv[1];
  string db_name = "demo.db";

  Emitter emitter(uri, Emitter::Method::POST, Emitter::Protocol::HTTP, 52000, 52000, 500, db_name);

  Subject subject;
  subject.set_user_id("a-user-id");
  subject.set_screen_resolution(1920, 1080);
  subject.set_viewport(1080, 1080);
  subject.set_color_depth(32);
  subject.set_timezone("GMT");
  subject.set_language("EN");
  subject.set_useragent("Mozilla/5.0");

  ClientSession client_session(db_name, 5000, 5000);

  string platform = "mob";
  string app_id = "app-id";
  string name_space = "namespace";
  bool base64 = false;
  bool desktop_context = true;

  // Create Tracker
  Tracker *t = Tracker::init(emitter, &subject, &client_session, &platform, &app_id, &name_space, &base64, &desktop_context);

  time_t start, end;
  time (&start);

  for (int i = 0; i < 2000; i++) {
    Tracker::TimingEvent te("timing-cat", "timing-var", 123);

    Tracker::ScreenViewEvent sve;
    string name = "Screen ID - 5asd56";
    sve.name = &name;

    Tracker::StructuredEvent se("shop", "add-to-basket");
    string property = "pcs";
    double value = 25.6;
    se.property = &property;
    se.value = &value;

    t->track_timing(te);
    t->track_screen_view(sve);
    t->track_struct_event(se);
  }

  time (&end);
  double diff = difftime (end,start);
  printf("It took me %f seconds to build and store 6000 events.\n", diff);

  // Flush and close
  t->flush();
  Tracker::close();

  time (&end);
  diff = difftime (end,start);
  printf("It took me %f seconds to send 6000 events.\n", diff);
  
  return 0;
}
