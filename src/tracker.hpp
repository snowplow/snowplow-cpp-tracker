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
using namespace std;

class Tracker {
private:
  Emitter & m_emitter;
public:
  void track(Payload p);
  Tracker(const string & url, Emitter & e);
  ~Tracker();
  void flush();
  void close();

  const string tracker_version = "cpp-0.1.0";

  // post requests
  const string post_protocol_vendor = "com.snowplowanalytics.snowplow";
  const string post_protocol_version = "tp2";
  const string post_content_type = "application/json; charset=utf-8";

  // get requests
  const string  get_protocol_path = "i";

  // schema versions
  const string schema_payload_data = "iglu:com.snowplowanalytics.snowplow/payload_data/jsonschema/1-0-4";
  const string schema_contexts = "iglu:com.snowplowanalytics.snowplow/contexts/jsonschema/1-0-1";
  const string schema_unstruct_event = "iglu:com.snowplowanalytics.snowplow/unstruct_event/jsonschema/1-0-0";
  const string schema_screen_view = "iglu:com.snowplowanalytics.snowplow/screen_view/jsonschema/1-0-0";
  const string schema_user_timings = "iglu:com.snowplowanalytics.snowplow/timing/jsonschema/1-0-0";

  // event types
  const string event_page_view = "pv";
  const string event_structured = "se";
  const string event_unstructured = "ue";
  const string event_ecomm = "tr";
  const string event_ecomm_item = "ti";

  // general
  const string schema = "schema";
  const string data = "data";
  const string event = "e";
  const string eid = "eid";
  const string timestamp = "dtm";
  const string sent_timestamp = "stm";
  const string true_timestamp = "ttm";
  const string t_version = "tv";
  const string app_id = "aid";
  const string sp_namespace = "tna";
  const string platform = "p";

  const string context = "co";
  const string context_encoded = "cx";
  const string unstructured = "ue_pr";
  const string unstructured_encoded = "ue_px";

  // subject class
  const string uid = "uid";
  const string resolution = "res";
  const string viewport = "vp";
  const string color_depth = "cd";
  const string timezone = "tz";
  const string language = "lang";
  const string ip_address = "ip";
  const string useragent = "ua";
  const string domain_uid = "duid";
  const string network_uid = "tnuid";

  // page view
  const string page_url = "url";
  const string page_title = "page";
  const string    page_refr = "refr";

  // structured event
  const string se_category = "se_ca";
  const string se_action = "se_ac";
  const string se_label = "se_la";
  const string se_property = "se_pr";
  const string se_value = "se_va";

  // ecomm transaction
  const string tr_id = "tr_id";
  const string tr_total = "tr_tt";
  const string tr_affiliation = "tr_af";
  const string tr_tax = "tr_tx";
  const string tr_shipping = "tr_sh";
  const string tr_city = "tr_ci";
  const string tr_state = "tr_st";
  const string tr_country = "tr_co";
  const string tr_currency = "tr_cu";

  // transaction item
  const string ti_item_id = "ti_id";
  const string ti_item_sku = "ti_sk";
  const string ti_item_name = "ti_nm";
  const string ti_item_category = "ti_ca";
  const string ti_item_price = "ti_pr";
  const string ti_item_quantity = "ti_qu";
  const string ti_item_currency = "ti_cu";

  // screen view
  const string sv_id = "id";
  const string sv_name = "name";

  // user timing
  const string ut_category = "category";
  const string ut_variable = "variable";
  const string ut_timing = "timing";
  const string  ut_label = "label";

};

#endif
