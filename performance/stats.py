#!/usr/bin/env python3

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

print()
print(''.join([to_cell('Metric', 40), to_cell('Max'), to_cell('Min'), to_cell('Mean'), to_cell('Last')]))
print(''.join(['-'] * 80))

for metric in metrics:
    values = [m['results'][metric] for m in measurements]
    print(''.join([
        to_cell(metric.replace('_', ' '), 40),
        to_cell(max(values)),
        to_cell(min(values)),
        to_cell(sum(values) / len(values)),
        to_cell(values[-1])
    ]))

print()
