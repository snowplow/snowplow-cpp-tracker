.PHONY: all tests lcov-genhtml clean test-clean dist-clean

build-dir = build/
test-dir = $(build-dir)test/
example-dir = $(build-dir)example/
performance-dir = $(build-dir)performance/
coverage-dir = $(build-dir)coverage/

test-name = tracker_test
example-name = tracker_example
performance-name = tracker_performance
coverage-name = coverage.info

# C Files

cc-include-files := $(shell find include -maxdepth 1 -name "*.c")
cc-objects := $(patsubst %.c, %.o, $(cc-include-files))

# C++ Files

cxx-src-files := $(shell find src -maxdepth 2 -name "*.cpp")
cxx-include-files := $(shell find include -maxdepth 1 -name "*.cpp")
cxx-test-files := $(shell find test -maxdepth 2 -name "*.cpp")
cxx-example-files := $(shell find examples -maxdepth 1 -name "*.cpp")
cxx-performance-files := $(shell find performance -maxdepth 1 -name "*.cpp")

cxx-common-objects := $(patsubst %.cpp, %.o, $(cxx-src-files) $(cxx-include-files))
cxx-test-objects := $(patsubst %.cpp, %.o, $(cxx-test-files))
cxx-example-objects := $(patsubst %.cpp, %.o, $(cxx-example-files))
cxx-performance-objects := $(patsubst %.cpp, %.o, $(cxx-performance-files))

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
# Objective-C++ Files

objcxx-src-files := $(shell find src -maxdepth 1 -name "*.mm")
objcxx-objects := $(patsubst %.mm, %.o, $(objcxx-src-files))
else
objcxx-src-files :=
objcxx-objects :=
endif

# Arguments

CC := gcc
CXX := g++
OBJCXX := c++
CCFLAGS := -Werror -g
CXXFLAGS := -std=c++11 -Werror -g -D SNOWPLOW_TEST_SUITE --coverage -O0
ifeq ($(UNAME_S), Darwin)
LDFLAGS := -framework CoreFoundation -framework CFNetwork -framework Foundation -framework CoreServices
LDLIBS := -lcurl
else
LDLIBS := -lcurl -pthread -ldl -luuid
LDFLAGS :=
endif

# Building

all: $(test-dir)$(test-name) $(example-dir)$(example-name) $(performance-dir)$(performance-name)

$(test-dir)$(test-name): $(objcxx-objects) $(cxx-common-objects) $(cxx-test-objects) $(cc-objects)
	mkdir -p $(test-dir)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(test-dir)$(test-name) $(objcxx-objects) $(cxx-common-objects) $(cxx-test-objects) $(cc-objects) $(LDLIBS)

$(example-dir)$(example-name): $(objcxx-objects) $(cxx-common-objects) $(cxx-example-objects) $(cc-objects)
	mkdir -p $(example-dir)
	$(CXX) -std=c++11 -Werror -g $(objcxx-src-files) $(cxx-src-files) $(cxx-include-files) $(cxx-example-files) $(LDFLAGS) -o $(example-dir)$(example-name) $(cc-objects) $(LDLIBS)

$(performance-dir)$(performance-name): $(objcxx-objects) $(cxx-common-objects) $(cxx-performance-objects) $(cc-objects)
	mkdir -p $(performance-dir)
	$(CXX) -std=c++11 -Werror -g $(objcxx-src-files) $(cxx-src-files) $(cxx-include-files) $(cxx-performance-files) $(LDFLAGS) -o $(performance-dir)$(performance-name) $(cc-objects) $(LDLIBS)

# Testing

tests: test-clean all
	(cd $(test-dir); ./$(test-name))

# Coverage

lcov-genhtml: tests
	mkdir -p $(coverage-dir)
	lcov --capture --directory src --output-file $(coverage-dir)$(coverage-name)
	genhtml $(coverage-dir)$(coverage-name) --output-directory $(coverage-dir) --demangle-cpp

# Dependencies

src/utils_macos.o: src/utils_macos.mm
	$(OBJCXX) $(CXXFLAGS) src/utils_macos.mm -c -o src/utils_macos.o

depend-cxx: .depend-cxx
.depend-cxx: $(cxx-src-files) $(cxx-test-files) $(cxx-include-files) $(cxx-example-files) $(cxx-performance-files)
	rm -f ./.depend-cxx
	$(CXX) $(CXXFLAGS) -MM $^ >> ./.depend-cxx;

depend-cc: .depend-cc
.depend-cc: $(cc-include-files)
	rm -f ./.depend-cc
	$(CC) $(CCFLAGS) -MM $^ >> ./.depend-cc;

# Cleanup

clean:
	rm -f $(objcxx-objects)
	rm -f $(cxx-common-objects)
	rm -f $(cxx-test-objects)
	rm -f $(cxx-example-objects)
	rm -f $(cxx-performance-objects)
	rm -f $(cc-objects)
	rm -f $(shell find . -maxdepth 3 -name "*.gcno")

test-clean:
	rm -f $(shell find . -maxdepth 3 -name "*.gcov")
	rm -f $(shell find . -maxdepth 3 -name "*.gcda")

dist-clean: test-clean clean
	rm -f *~ .depend-cxx
	rm -f *~ .depend-cc
	rm -rf build/coverage
	rm -rf build/example
	rm -rf build/performance
	rm -rf build/test

include .depend-cxx
include .depend-cc
