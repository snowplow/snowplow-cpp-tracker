/*
Copyright (c) 2022 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#ifndef SNOWPLOW_H
#define SNOWPLOW_H

/**
 * @brief This file provides the single header to import when using the Snowplow tracker in your code.
 */

#include "client_session.hpp"
#include "emitter.hpp"
#include "subject.hpp"
#include "tracker.hpp"

// storage
#include "storage/event_row.hpp"
#include "storage/event_store.hpp"
#include "storage/session_store.hpp"
#include "storage/sqlite_storage.hpp"

// http
#include "http/http_client.hpp"
#include "http/http_client_apple.hpp"
#include "http/http_client_curl.hpp"
#include "http/http_client_windows.hpp"

// payload
#include "payload/payload.hpp"
#include "payload/self_describing_json.hpp"

// events
#include "events/event.hpp"
#include "events/screen_view_event.hpp"
#include "events/self_describing_event.hpp"
#include "events/structured_event.hpp"
#include "events/timing_event.hpp"

#endif
