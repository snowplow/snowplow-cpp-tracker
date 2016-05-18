#include <iostream>
#include <string>

#include "../src/tracker.hpp"

using namespace std;

int main(int argc, char** argv) {
  Emitter e("52.30.36.95:8080", Emitter::Method::POST, Emitter::HTTP, 5000, 5000, 5000, "demo.db");
  Tracker t(e, NULL, NULL, NULL, NULL, NULL);

  string my_schema = "schema";
  json data = "{\"hello\":\"world\"}"_json;

  Tracker::SelfDescribingEvent sde(SelfDescribingJson(my_schema,data));
  t.track_self_describing_event(sde);

  t.flush();
  return 0;
}
