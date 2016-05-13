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

Tracker::Tracker(const string & url, Emitter & e) : m_emitter(e), m_subject() {
  e.start();
}

void Tracker::track(Payload p, vector<SelfDescribingJson> & contexts) {
  //p.add(&this->tracker_version,  )
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
