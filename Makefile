.PHONY: all unit-tests clean dist-clean

build-dir = build/
app-name  = tracker_test.o

# C Files

cc-include-files := $(shell find include -maxdepth 1 -name "*.c")
cc-objects := $(patsubst %.c, %.o, $(cc-include-files))

# C++ Files

cxx-src-files := $(shell find src -maxdepth 1 -name "*.cpp")
cxx-test-files := $(shell find test -maxdepth 1 -name "*.cpp")
cxx-include-files := $(shell find include -maxdepth 1 -name "*.cpp")
cxx-objects := $(patsubst %.cpp, %.o, $(cxx-src-files) $(cxx-test-files) $(cxx-include-files))

# Combined Objects

objects := $(cxx-objects) $(cc-objects)

# Arguments

CC := gcc
CXX := g++
CCFLAGS := -Werror -g
CXXFLAGS := -std=c++11 -Werror -g -D SNOWPLOW_TEST_SUITE
LDFLAGS := -framework CoreFoundation -framework CFNetwork

# Building

all: $(build-dir)$(app-name)

$(build-dir)$(app-name): $(objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(build-dir)$(app-name) $(objects) $(LDLIBS)

depend-cxx: .depend-cxx
.depend-cxx: $(cxx-src-files) $(cxx-test-files) $(cxx-include-files)
	rm -f ./.depend-cxx
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend-cxx;

depend-cc: .depend-cc
.depend-cc: $(cc-include-files)
	rm -f ./.depend-cc
	$(CC) $(CCFLAGS) -MM $^>>./.depend-cc;

# Testing

unit-tests: all
	(cd $(build-dir); ./$(app-name))

# Cleanup

clean:
	rm -f $(objects)
	rm -f $(build-dir)$(app-name)

dist-clean: clean
	rm -f *~ .depend-cxx
	rm -f *~ .depend-cc

include .depend-cxx
include .depend-cc
