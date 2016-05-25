.PHONY: all unit-tests lcov-genhtml clean test-clean dist-clean

build-dir = build/
test-dir = $(build-dir)test/
example-dir = $(build-dir)example/
coverage-dir = $(build-dir)coverage/

test-name = tracker_test
example-name = tracker_example
coverage-name = coverage.info

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
CXXFLAGS := -std=c++11 -Werror -g -D SNOWPLOW_TEST_SUITE --coverage -O0
LDFLAGS := -framework CoreFoundation -framework CFNetwork

# Building

all: $(test-dir)$(test-name) $(example-dir)$(example-name)

$(test-dir)$(test-name): $(cxx-common-objects) $(cxx-test-objects) $(cc-objects)
	mkdir -p $(test-dir)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(test-dir)$(test-name) $(cxx-common-objects) $(cxx-test-objects) $(cc-objects) $(LDLIBS)

$(example-dir)$(example-name): $(cxx-common-objects) $(cxx-example-objects) $(cc-objects)
	mkdir -p $(example-dir)
	$(CXX) -std=c++11 -Werror -g $(cxx-src-files) $(cxx-include-files) $(cxx-example-files) $(LDFLAGS) -o $(example-dir)$(example-name) $(cc-objects) $(LDLIBS)

# Testing

unit-tests: test-clean all
	(cd $(test-dir); ./$(test-name))

# Coverage

lcov-genhtml: unit-tests
	mkdir -p $(coverage-dir)
	lcov --capture --directory src --output-file $(coverage-dir)$(coverage-name)
	genhtml $(coverage-dir)$(coverage-name) --output-directory $(coverage-dir) --demangle-cpp

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
	rm -f $(shell find . -maxdepth 3 -name "*.gcno")

test-clean:
	rm -f $(shell find . -maxdepth 3 -name "*.gcov")
	rm -f $(shell find . -maxdepth 3 -name "*.gcda")

dist-clean: test-clean clean
	rm -f *~ .depend-cxx
	rm -f *~ .depend-cc

include .depend-cxx
include .depend-cc
