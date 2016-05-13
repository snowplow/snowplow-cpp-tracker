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
  e.start();
  //this->m_has_subject = false;
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
