#!/usr/bin/env python3

# Copyright (c) 2022 Snowplow Analytics Ltd. All rights reserved.
#
# This program is licensed to you under the Apache License Version 2.0,
# and you may not use this file except in compliance with the Apache License Version 2.0.
# You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the Apache License Version 2.0 is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.

import json

def to_cell(text, width=10):
    if isinstance(text, float):
        text = str(round(text * 100) / 100) + 's'
    return ' ' + text.ljust(width - 2) + '|'

filename = 'performance/logs.txt'
with open(filename) as file:
    measurements = [json.loads(line.strip()) for line in file if line.strip()]

metrics = [
  'mocked_emitter_and_mocked_session',
  'mocked_emitter_and_real_session',
  'mute_emitter_and_mocked_session',
  'mute_emitter_and_real_session'
]

groups = {
    (m['desktop_context']['data']['deviceModel'], m['results']['num_threads'], m['results']['num_operations'])
    for m in measurements
}

for device_model, num_threads, num_operations in groups:
    group_measurements = [
        m for m in measurements
        if m['desktop_context']['data']['deviceModel'] == device_model
        and m['results']['num_threads'] == num_threads
        and m['results']['num_operations'] == num_operations
    ]
    print(f'Device: {device_model}')
    print(f'Number of threads: {num_threads}')
    print(f'Number of operations: {num_operations}')
    print()
    print(''.join([to_cell('Metric', 40), to_cell('Max'), to_cell('Min'), to_cell('Mean'), to_cell('Last')]))
    print(''.join(['-'] * 80))

    for metric in metrics:
        values = [m['results'][metric] for m in group_measurements]
        print(''.join([
            to_cell(metric.replace('_', ' '), 40),
            to_cell(max(values)),
            to_cell(min(values)),
            to_cell(sum(values) / len(values)),
            to_cell(values[-1])
        ]))

    print()
