#include <iostream>
#include <string>

#include "../src/tracker.hpp"

using namespace std;

int main(int argc, char** argv) {
  // NB - you can't use _http:// here! The url gets packed out as "//host" which breaks it
  // also - we don't need two urls do we, which one is right?

  // POST requests are broken?
  // the tests in http_client_test pass on win

  Emitter e("c91c801c.ngrok.io", Emitter::Method::POST, Emitter::HTTP, 5000, 5000, 5000, "demo.db");
  string url = "c91c801c.ngrok.io";
  Tracker t(url, e);

  string my_schema = "schema";
  json data = "{\"hello\":\"world\"}"_json;

  Tracker::SelfDescribingEvent sde(SelfDescribingJson(my_schema,data));
  t.track_self_describing_event(sde);

  t.close();

  cin.ignore();
  }