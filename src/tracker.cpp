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

#include "tracker.hpp"

Tracker::Tracker(string & url, Emitter & e) : m_emitter(e), m_subject() {
  // set defaults
  this->m_has_subject = false;
  this->m_platform = "srv";
  this->m_app_id = "";
  this->m_use_base64 = true;
  this->m_namespace = "";

  // start the emitter daemon if it's not started already
  e.start();
}

void Tracker::track(Payload payload, vector<SelfDescribingJson> & contexts) {

  payload.add(SNOWPLOW_TRACKER_VERSION, SNOWPLOW_TRACKER_VERSION_LABEL);
  payload.add(SNOWPLOW_PLATFORM, this->m_platform);
  payload.add(SNOWPLOW_APP_ID, this->m_app_id);
  payload.add(SNOWPLOW_SP_NAMESPACE, this->m_namespace);

  if (m_has_subject) {
    payload.add_map(m_subject.get_map());
  }

  if (contexts.size() > 0) {
    for (int i = 0; i < contexts.size(); i++) {
      payload.add_json(contexts[i].get(), m_use_base64, SNOWPLOW_CONTEXT_ENCODED, SNOWPLOW_CONTEXT);
    }
  }

  this->m_emitter.add(payload);
}

//void Tracker::track_page_view_event(PageViewEvent p) {
//
//  if (p.page_url == "") { throw invalid_argument("Page view URL cannot be empty"); }
//
//  Payload payload;
//  payload.add(SNOWPLOW_EVENT, SNOWPLOW_EVENT_PAGE_VIEW);
//  payload.add(SNOWPLOW_PAGE_URL, p.page_url);
//  
//  if (p.timestamp > 0) {
//    payload.add(SNOWPLOW_TIMESTAMP, to_string(p.timestamp));
//  }
//
//  payload.add(SNOWPLOW_EID, p.event_id);
//
//  if (p.true_timestamp > 0) {
//    payload.add(SNOWPLOW_TRUE_TIMESTAMP, to_string(p.true_timestamp));
//  }
//  
//  payload.add(SNOWPLOW_PAGE_REFR, p.referrer);
//  payload.add(SNOWPLOW_PAGE_TITLE, p.page_title);
//
//  track(payload, p.contexts);
//}

void Tracker::track_struct_event(StructuredEvent se) {
  if (se.action == "") { throw invalid_argument("Action is required"); }
  if (se.category == "") { throw invalid_argument("Category is required"); }

  Payload p;
  p.add(SNOWPLOW_EVENT, SNOWPLOW_EVENT_STRUCTURED);
  p.add(SNOWPLOW_SE_ACTION, se.action);
  p.add(SNOWPLOW_SE_CATEGORY, se.category);

  if (se.timestamp > 0) {
    p.add(SNOWPLOW_TIMESTAMP, to_string(se.timestamp));
  }

  if (se.true_timestamp != 0) {
    p.add(SNOWPLOW_TRUE_TIMESTAMP, to_string(se.true_timestamp));
  }

  p.add(SNOWPLOW_EID, se.event_id);
  p.add(SNOWPLOW_SE_LABEL, se.label);
  p.add(SNOWPLOW_SE_PROPERTY, se.property);
  p.add(SNOWPLOW_SE_VALUE, to_string(se.value));

  track(p, se.contexts);
}

void Tracker::flush()
{
  m_emitter.flush();
}

void Tracker::close()
{
  m_emitter.stop();
}

Tracker::~Tracker()
{
  close();
}
//
//Tracker::PageViewEvent::PageViewEvent(string page_url) {
//  this->page_url = page_url;
//  this->page_title = "";
//  this->event_id = Utils::get_uuid4();
//  this->referrer = "";
//  this->timestamp = Utils::get_unix_epoch_ms();
//  this->true_timestamp = 0;
//  this->contexts = vector<SelfDescribingJson>();
//}

Tracker::StructuredEvent::StructuredEvent(string category, string action) {
  this->category = category;
  this->action = action;
  this->contexts = vector<SelfDescribingJson>();
  this->event_id = Utils::get_uuid4();
  this->timestamp = Utils::get_unix_epoch_ms();
  this->true_timestamp = 0;
  this->label = "";
  this->property = "";
  this->value = 0.0;
}
//
//Tracker::SelfDescribingEvent::SelfDescribingEvent(SelfDescribingJson event): event(event) {
//  this->event_id = Utils::get_uuid4();
//  this->timestamp = Utils::get_unix_epoch_ms();
//  this->true_timestamp = 0;
//  this->contexts = vector<SelfDescribingJson>();
//}

Tracker::ScreenViewEvent::ScreenViewEvent() {
  this->contexts = vector<SelfDescribingJson>();
  this->event_id = Utils::get_uuid4();
  this->timestamp = Utils::get_unix_epoch_ms();
  this->true_timestamp = 0;
  this->id = "";
  this->name = "";
}

Tracker::TimingEvent::TimingEvent(string category, string variable) {
  this->category = category;
  this->variable = variable;
  this->timestamp = Utils::get_unix_epoch_ms();
  this->true_timestamp = 0;
  this->timing = 0;
  this->contexts = vector<SelfDescribingJson>();
  this->event_id = Utils::get_uuid4();
  this->label = "";
}
//
//Tracker::EcommerceTransactionItemEvent::EcommerceTransactionItemEvent(string sku, double price) {
//  this->sku = sku;
//  this->price = price;
//  this->event_id = Utils::get_uuid4();
//  this->quantity = 1;
//  this->contexts = vector<SelfDescribingJson>();
//  this->name = "";
//  this->category = "";
//}
//
//Tracker::EcommerceTransactionEvent::EcommerceTransactionEvent(string order_id, double total_value) {
//  this->order_id = order_id;
//  this->total_value = total_value;
//  this->items = vector<EcommerceTransactionItemEvent>();
//  this->affiliation = "";
//  this->city = "";
//  this->contexts = vector<SelfDescribingJson>();
//  this->country = "";
//  this->currency = "";
//  this->event_id = Utils::get_uuid4();
//  this->timestamp = Utils::get_unix_epoch_ms();
//  this->shipping = 0.0;
//  this->state = "";
//  this->tax_value = 0.0;
//  this->true_timestamp = 0;
//}