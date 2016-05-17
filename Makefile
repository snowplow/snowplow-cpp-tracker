.PHONY: all clean dist-clean

appname = tracker_test.o

CXX := g++
CXXFLAGS := -std=c++11
LDFLAGS = -l sqlite3 -framework CoreFoundation -framework CFNetwork

srcfiles := $(shell find src -maxdepth 2 -name "*.cpp")
objects  := $(patsubst %.cpp, %.o, $(srcfiles))

all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname) $(objects) $(LDLIBS)

depend: .depend

.depend: $(srcfiles)
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
