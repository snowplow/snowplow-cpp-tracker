/*
Copyright (c) 2016 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#ifndef TRACKER_H
#define TRACKER_H

#include <string>
#include "emitter.hpp"
#include "subject.hpp"
#include "self_describing_json.hpp"

using namespace std;

class Tracker {
private:
  Emitter & m_emitter;
  Subject m_subject;
  bool m_has_subject;
  string m_namespace;
  string m_app_id;
  string m_platform;
  bool m_use_base64;

public:

  //class PageViewEvent {
  //public:
  //  string page_url; // required
  //  string page_title; // optional from here
  //  string referrer;
  //  unsigned long long timestamp;
  //  string event_id;
  //  unsigned long long true_timestamp;
  //  vector<SelfDescribingJson> contexts;

  //  PageViewEvent(string);
  //};

  class StructuredEvent {
  public:
    string category; // required
    string action; // required
    string *label;
    string *property;
    double *value;
    unsigned long long timestamp;
    string event_id;
    unsigned long long *true_timestamp;
    vector<SelfDescribingJson> contexts;

    StructuredEvent(string, string); // cat / action
  };

  class SelfDescribingEvent {
  public:
    SelfDescribingJson event; // required
    unsigned long long timestamp;
    string event_id;
    unsigned long long *true_timestamp;
    vector<SelfDescribingJson> contexts;

    SelfDescribingEvent(SelfDescribingJson);
  };

  class ScreenViewEvent {
  public:
    string *name;
    string *id;
    unsigned long long timestamp;
    string event_id;
    unsigned long long *true_timestamp;
    vector<SelfDescribingJson> contexts;

    ScreenViewEvent();
  };

  class TimingEvent {
  public:
    string category; // required
    string variable; // required
    unsigned long long timing;
    string *label;
    unsigned long long timestamp;
    string event_id;
    unsigned long long *true_timestamp;
    vector<SelfDescribingJson> contexts;

    TimingEvent(string, string, unsigned long long);
  };

  //class EcommerceTransactionItemEvent {
  //public:
  //  string sku; // required
  //  double price; // required
  //  long quantity;
  //  string name;
  //  string category;
  //  string event_id;
  //  vector<SelfDescribingJson> contexts;

  //  EcommerceTransactionItemEvent(string, double);
  //};

  //class EcommerceTransactionEvent {
  //public:
  //  string order_id; // required
  //  double total_value; // required
  //  string affiliation;
  //  double tax_value;
  //  double shipping;
  //  string city;
  //  string state;
  //  string country;
  //  string currency;
  //  vector<EcommerceTransactionItemEvent> items;
  //  unsigned long long timestamp;
  //  string event_id;
  //  unsigned long long true_timestamp;
  //  vector<SelfDescribingJson> contexts;

  //  EcommerceTransactionEvent(string, double);
  //};

  Tracker(string & url, Emitter & e);
  ~Tracker();

  void flush();
  void close();
  
  void track(Payload p, vector<SelfDescribingJson> & contexts);
  //void track_page_view_event(PageViewEvent);
  void track_struct_event(StructuredEvent);
  //void track_self_describing_event(SelfDescribingEvent);
  void track_screen_view(ScreenViewEvent);
  void track_timing(TimingEvent);
  void track_unstruct_event(SelfDescribingEvent e);
  //void track_ecomerce_transaction(EcommerceTransactionEvent);
  //void track_ecomerce_transaction_item(EcommerceTransactionItemEvent, string, string, unsigned long long, unsigned long long);

};

#endif
