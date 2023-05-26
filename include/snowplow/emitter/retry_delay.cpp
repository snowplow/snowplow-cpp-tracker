/*
Copyright (c) 2023 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#include "retry_delay.hpp"
#include <random>

using namespace snowplow;
using std::min;

RetryDelay::RetryDelay(double base, double factor, int retry_count_cap, double jitter) {
  m_base = base;
  m_factor = factor;
  m_retry_count_cap = retry_count_cap;
  m_jitter = jitter;
  m_retry_count = 0;
}

void RetryDelay::will_retry_emit() {
  m_retry_count++;
}

void RetryDelay::wont_retry_emit() {
  m_retry_count = 0;
}

milliseconds RetryDelay::get() const {
  if (m_retry_count == 0) {
    return milliseconds(0);
  }

  double delay_ms = m_base * pow(m_factor, min(m_retry_count, m_retry_count_cap) - 1);

  if (m_jitter != 0) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double seed = dist(mt);
    double deviation = floor(seed * m_jitter * delay_ms);

    if (round(seed) == 1) {
      delay_ms -= deviation;
    } else {
      delay_ms += deviation;
    }
  }

  return milliseconds((unsigned long) delay_ms);
}
