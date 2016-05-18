.PHONY: all unit-tests clean dist-clean

build-dir = build/
app-name  = tracker_test.o

src-files     := $(shell find src -maxdepth 1 -name "*.cpp")
test-files    := $(shell find test -maxdepth 1 -name "*.cpp")
include-files := $(shell find include -maxdepth 1 -name "*.cpp")
objects       := $(patsubst %.cpp, %.o, $(src-files) $(test-files) $(include-files))

CXX      := g++
CXXFLAGS := -std=c++11 -Werror -g -D SNOWPLOW_TEST_SUITE
LDFLAGS  = -l sqlite3 -framework CoreFoundation -framework CFNetwork

# Building

all: $(build-dir)$(app-name)

$(build-dir)$(app-name): $(objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(build-dir)$(app-name) $(objects) $(LDLIBS)

depend: .depend

.depend: $(src-files) $(test-files) $(include-files)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

# Testing

unit-tests: all
	(cd $(build-dir); ./$(app-name))

# Cleanup

clean:
	rm -f $(objects)

dist-clean: clean
	rm -f *~ .depend

include .depend
