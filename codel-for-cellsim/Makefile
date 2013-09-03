source = payload.cc select.cc packetsocket.cc cell-sched.cc delay-queue.cc timestamp.cc pkt-classifier.cc las.cc codel.cc sfqCoDel.cc
objects = payload.o select.o packetsocket.o   timestamp.o delay-queue.o pkt-classifier.o las.o codel.o sfqCoDel.o
executables = cell-sched

CXX = g++
CXXFLAGS = -g -O3 -std=c++0x -ffast-math -pedantic -Wall -Wextra -Werror -Weffc++ -fno-default-inline -pipe -D_FILE_OFFSET_BITS=64 -D_XOPEN_SOURCE=500 -D_GNU_SOURCE
LIBS = -lm -lrt

all: $(executables)

cell-sched: cell-sched.o $(objects)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

-include depend

depend: $(source)
	$(CXX) $(INCLUDES) -MM $(source) > depend

.PHONY: clean
clean:
	-rm -f $(executables) depend *.o *.rpo
