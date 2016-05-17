.PHONY: all clean dist-clean

name = tracker_test.o
files := $(shell find src -maxdepth 2 -name "*.cpp")
objects  := $(patsubst %.cpp, %.o, $(files))

CXX := g++
CXXFLAGS := -std=c++11
LDFLAGS = -l sqlite3 -framework CoreFoundation -framework CFNetwork

all: $(name)

$(name): $(objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(name) $(objects) $(LDLIBS)

depend: .depend

.depend: $(files)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	rm -f $(objects)
	rm -f test.db
	rm -f test.db-shm
	rm -f test.db-wal

dist-clean: clean
	rm -f *~ .depend

include .depend
