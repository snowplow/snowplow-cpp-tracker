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

#ifndef RETRY_DELAY_H
#define RETRY_DELAY_H

#include <chrono>

namespace snowplow {

using std::chrono::milliseconds;

/**
 * @brief Calculates exponential retry delay for Emitter based on the number of retry attempts.
 */
class RetryDelay {
public:
  /**
   * @brief Construct a new RetryDely object
   *
   * @param base Base delay in milliseconds
   * @param factor Multiplicative factor for exponential function
   * @param retry_count_cap Maximum retry count to consider in the retry delay calculation
   * @param jitter Amount of randomness to introduce in the calculation (from 0 to 1)
   */
  RetryDelay(double base = 100.0, double factor = 2.0, int retry_count_cap = 10, double jitter = 0.1);

  /**
   * @brief Update retry delay considering that a new retry is planned.
   */
  void will_retry_emit();

  /**
   * @brief Update retry delay considering that no more retries are planned.
   */
  void wont_retry_emit();

  milliseconds get() const;

private:
  int m_retry_count;
  double m_base;
  double m_factor;
  int m_retry_count_cap;
  double m_jitter;
};
} // namespace snowplow

#endif
