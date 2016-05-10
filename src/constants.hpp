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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

using namespace std;

const string TRACKER_VERSION_LABEL = "cpp-0.1.0";

// post requests
const string POST_PROTOCOL_VENDOR = "com.snowplowanalytics.snowplow";
const string POST_PROTOCOL_VERSION = "tp2";
const string POST_CONTENT_TYPE = "application/json; charset=utf-8";

// get requests
const string GET_PROTOCOL_PATH = "i";

// schema versions
const string SCHEMA_PAYLOAD_DATA = "iglu:com.snowplowanalytics.snowplow/payload_data/jsonschema/1-0-4";
const string SCHEMA_CONTEXTS = "iglu:com.snowplowanalytics.snowplow/contexts/jsonschema/1-0-1";
const string SCHEMA_UNSTRUCT_EVENT = "iglu:com.snowplowanalytics.snowplow/unstruct_event/jsonschema/1-0-0";
const string SCHEMA_SCREEN_VIEW = "iglu:com.snowplowanalytics.snowplow/screen_view/jsonschema/1-0-0";
const string SCHEMA_USER_TIMINGS = "iglu:com.snowplowanalytics.snowplow/timing/jsonschema/1-0-0";

// event types
const string EVENT_STRUCTURED = "se";
const string EVENT_UNSTRUCTURED = "ue";

const string CONTEXT = "co";
const string CONTEXT_ENCODED = "cx";
const string UNSTRUCTURED = "ue_pr";
const string UNSTRUCTURED_ENCODED = "ue_px";

// general
const string SCHEMA = "schema";
const string DATA = "data";
const string EVENT = "e";
const string EID = "eid";
const string TIMESTAMP = "dtm";
const string SENT_TIMESTAMP = "stm";
const string TRUE_TIMESTAMP = "ttm";
const string TRACKER_VERSION = "tv";
const string APP_ID = "aid";
const string SP_NAMESPACE = "tna";
const string PLATFORM = "p";

// subject class
const string UID = "uid";
const string RESOLUTION = "res";
const string VIEWPORT = "vp";
const string COLOR_DEPTH = "cd";
const string TIMEZONE = "tz";
const string LANGUAGE = "lang";

// structured event
const string SE_CATEGORY = "se_ca";
const string SE_ACTION = "se_ac";
const string SE_LABEL = "se_la";
const string SE_PROPERTY = "se_pr";
const string SE_VALUE = "se_va";

// screen view
const string SV_ID = "id";
const string SV_NAME = "name";

// user timing
const string UT_CATEGORY = "category";
const string UT_VARIABLE = "variable";
const string UT_TIMING = "timing";
const string UT_LABEL = "label";

#endif
