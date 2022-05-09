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

#ifndef EMIT_STATUS_H
#define EMIT_STATUS_H

namespace snowplow {

enum EmitStatus {
  SUCCESS = 1,
  FAILED_WILL_RETRY = 2,
  FAILED_WONT_RETRY = 4
};

inline EmitStatus operator|(EmitStatus a, EmitStatus b) {
  return static_cast<EmitStatus>(static_cast<int>(a) | static_cast<int>(b));
}

} // namespace snowplow

#endif
