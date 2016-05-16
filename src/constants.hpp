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

const string SNOWPLOW_TRACKER_VERSION_LABEL = "cpp-0.1.0";

// post requests
const string SNOWPLOW_POST_PROTOCOL_VENDOR = "com.snowplowanalytics.snowplow";
const string SNOWPLOW_POST_PROTOCOL_VERSION = "tp2";
const string SNOWPLOW_POST_CONTENT_TYPE = "application/json; charset=utf-8";

// get requests
const string SNOWPLOW_GET_PROTOCOL_PATH = "i";

// schema versions
const string SNOWPLOW_SCHEMA_PAYLOAD_DATA = "iglu:com.snowplowanalytics.snowplow/payload_data/jsonschema/1-0-4";
const string SNOWPLOW_SCHEMA_CONTEXTS = "iglu:com.snowplowanalytics.snowplow/contexts/jsonschema/1-0-1";
const string SNOWPLOW_SCHEMA_UNSTRUCT_EVENT = "iglu:com.snowplowanalytics.snowplow/unstruct_event/jsonschema/1-0-0";
const string SNOWPLOW_SCHEMA_SCREEN_VIEW = "iglu:com.snowplowanalytics.snowplow/screen_view/jsonschema/1-0-0";
const string SNOWPLOW_SCHEMA_USER_TIMINGS = "iglu:com.snowplowanalytics.snowplow/timing/jsonschema/1-0-0";
const string SNOWPLOW_SCHEMA_CLIENT_SESSION = "iglu:com.snowplowanalytics.snowplow/client_session/jsonschema/1-0-1";

// event types
const string SNOWPLOW_EVENT_STRUCTURED = "se";
const string SNOWPLOW_EVENT_UNSTRUCTURED = "ue";
const string SNOWPLOW_EVENT_PAGE_VIEW = "pv";

const string SNOWPLOW_CONTEXT = "co";
const string SNOWPLOW_CONTEXT_ENCODED = "cx";
const string SNOWPLOW_UNSTRUCTURED = "ue_pr";
const string SNOWPLOW_UNSTRUCTURED_ENCODED = "ue_px";

// general
const string SNOWPLOW_SCHEMA = "schema";
const string SNOWPLOW_DATA = "data";
const string SNOWPLOW_EVENT = "e";
const string SNOWPLOW_EID = "eid";
const string SNOWPLOW_TIMESTAMP = "dtm";
const string SNOWPLOW_SENT_TIMESTAMP = "stm";
const string SNOWPLOW_TRUE_TIMESTAMP = "ttm";
const string SNOWPLOW_TRACKER_VERSION = "tv";
const string SNOWPLOW_APP_ID = "aid";
const string SNOWPLOW_SP_NAMESPACE = "tna";
const string SNOWPLOW_PLATFORM = "p";

// subject class
const string SNOWPLOW_UID = "uid";
const string SNOWPLOW_RESOLUTION = "res";
const string SNOWPLOW_VIEWPORT = "vp";
const string SNOWPLOW_COLOR_DEPTH = "cd";
const string SNOWPLOW_TIMEZONE = "tz";
const string SNOWPLOW_LANGUAGE = "lang";

// structured event
const string SNOWPLOW_SE_CATEGORY = "se_ca";
const string SNOWPLOW_SE_ACTION = "se_ac";
const string SNOWPLOW_SE_LABEL = "se_la";
const string SNOWPLOW_SE_PROPERTY = "se_pr";
const string SNOWPLOW_SE_VALUE = "se_va";

// page view
const string SNOWPLOW_PAGE_URL = "url";
const string SNOWPLOW_PAGE_TITLE = "page";
const string SNOWPLOW_PAGE_REFR = "refr";

// screen view
const string SNOWPLOW_SV_ID = "id";
const string SNOWPLOW_SV_NAME = "name";

// user timing
const string SNOWPLOW_UT_CATEGORY = "category";
const string SNOWPLOW_UT_VARIABLE = "variable";
const string SNOWPLOW_UT_TIMING = "timing";
const string SNOWPLOW_UT_LABEL = "label";

// client session context
const string SNOWPLOW_SESSION_USER_ID = "userId";
const string SNOWPLOW_SESSION_ID = "sessionId";
const string SNOWPLOW_SESSION_PREVIOUS_ID = "previousSessionId";
const string SNOWPLOW_SESSION_INDEX = "sessionIndex";
const string SNOWPLOW_SESSION_STORAGE = "storageMechanism";
const string SNOWPLOW_SESSION_FIRST_ID = "firstEventId";

#endif
