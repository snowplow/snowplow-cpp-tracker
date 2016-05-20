.PHONY: all unit-tests clean dist-clean

build-dir = build/
test-name = tracker_test
example-name = tracker_example

# C Files

cc-include-files := $(shell find include -maxdepth 1 -name "*.c")
cc-objects := $(patsubst %.c, %.o, $(cc-include-files))

# C++ Files

cxx-src-files := $(shell find src -maxdepth 1 -name "*.cpp")
cxx-include-files := $(shell find include -maxdepth 1 -name "*.cpp")
cxx-test-files := $(shell find test -maxdepth 1 -name "*.cpp")
cxx-example-files := $(shell find examples -maxdepth 1 -name "*.cpp")

cxx-common-objects := $(patsubst %.cpp, %.o, $(cxx-src-files) $(cxx-include-files))
cxx-test-objects := $(patsubst %.cpp, %.o, $(cxx-test-files))
cxx-example-objects := $(patsubst %.cpp, %.o, $(cxx-example-files))

# Arguments

CC := gcc
CXX := g++
CCFLAGS := -Werror -g
CXXFLAGS := -std=c++11 -Werror -g -D SNOWPLOW_TEST_SUITE
LDFLAGS := -framework CoreFoundation -framework CFNetwork

# Building

all: $(build-dir)$(test-name) $(build-dir)$(example-name)

$(build-dir)$(test-name): $(cxx-common-objects) $(cxx-test-objects) $(cc-objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(build-dir)$(test-name) $(cxx-common-objects) $(cxx-test-objects) $(cc-objects) $(LDLIBS)

$(build-dir)$(example-name): $(cxx-common-objects) $(cxx-example-objects) $(cc-objects)
	$(CXX) -std=c++11 -Werror -g $(cxx-src-files) $(cxx-include-files) $(cxx-example-files) $(LDFLAGS) -o $(build-dir)$(example-name) $(cc-objects) $(LDLIBS)

# Testing

unit-tests: all
	(cd $(build-dir); ./$(test-name))

# Dependencies

depend-cxx: .depend-cxx
.depend-cxx: $(cxx-src-files) $(cxx-test-files) $(cxx-include-files) $(cxx-example-files)
	rm -f ./.depend-cxx
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend-cxx;

depend-cc: .depend-cc
.depend-cc: $(cc-include-files)
	rm -f ./.depend-cc
	$(CC) $(CCFLAGS) -MM $^>>./.depend-cc;

# Cleanup

clean:
	rm -f $(cxx-common-objects)
	rm -f $(cxx-test-objects)
	rm -f $(cxx-example-objects)
	rm -f $(cc-objects)

dist-clean: clean
	rm -f *~ .depend-cxx
	rm -f *~ .depend-cc

include .depend-cxx
include .depend-cc
