# snowplow-cpp-tracker
Snowplow event tracker for C++. Add analytics to your C++ applications, servers and games

```bash
## OSX
g++ \
  -std=c++11 \
  -l sqlite3 \
  src/vendored/*.cpp \
  src/*.cpp \
  src/test/*.cpp \
  -o build/test && \
  ./build/test
```

General layout:

tracker; Subject
subject; screen res -> Optional
emitter; Storage
storage/event store;

Has to be singleton

tracker.Track -> Uses main thread (or new thread) -> Writes to database;
check database for range of events -> send them all on indivdual threads;
