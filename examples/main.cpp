#include <iostream>
#include <string>

#include "../src/tracker.hpp"

using namespace std;

int main(int argc, char** argv) {
  // NB - you can't use _http:// here! The url gets packed out as "//host" which breaks it
  // also - we don't need two urls do we, which one is right?

  // POST requests are broken?
  // the tests in http_client_test pass on win

  Emitter e("ec2-52-49-138-71.eu-west-1.compute.amazonaws.com:8080", Emitter::Method::GET, Emitter::HTTP, 5000, 5000, 5000, "demo.db");
  string url = "ec2-52-49-138-71.eu-west-1.compute.amazonaws.com:8080";
  Tracker t(url, e);

  string my_schema = "schema";
  json data = "{\"hello\":\"world\"}"_json;

  Tracker::SelfDescribingEvent sde(SelfDescribingJson(my_schema,data));
  t.track_unstruct_event(sde);

  t.close();

  cin.ignore();
  }