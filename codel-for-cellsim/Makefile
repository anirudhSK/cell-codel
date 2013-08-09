source = payload.cc select.cc packetsocket.cc cell-codel-sim.cc delay-queue.cc time-stamp.cc pkt-classifer.cc
objects = payload.o select.o packetsocket.o   timestamp.o delay-queue.o pkt-classifier.o
executables = cell-codel-sim

CXX = g++
CXXFLAGS = -g -O3 -std=c++0x -ffast-math -pedantic -Wall -Wextra -Werror -Weffc++ -fno-default-inline -pipe -D_FILE_OFFSET_BITS=64 -D_XOPEN_SOURCE=500 -D_GNU_SOURCE
LIBS = -lm -lrt

all: $(executables)

cell-codel-sim: cell-codel-sim.o $(objects) 
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

-include depend

depend: $(source)
	$(CXX) $(INCLUDES) -MM $(source) > depend

.PHONY: clean
clean:
	-rm -f $(executables) depend *.o *.rpo
