source = packet.cc slotted-link.cc slotted-sender.cc random.cc distribution.cc scheduling-algms/rr-scheduler.cc scheduling-algms/vclock-scheduler.cc scheduling-algms/fcfs-scheduler.cc scheduling-algms/drr-scheduler.cc scheduling-algms/tail-scheduler.cc scheduling-algms/qdelay-estimator.cc scheduling-algms/codel.cc
objects = packet.o slotted-link.o slotted-sender.o random.o distribution.o scheduling-algms/rr-scheduler.o scheduling-algms/vclock-scheduler.o scheduling-algms/fcfs-scheduler.o scheduling-algms/drr-scheduler.o scheduling-algms/tail-scheduler.o scheduling-algms/qdelay-estimator.o scheduling-algms/codel.o
executables = schism
CXX = g++
CXXFLAGS = -g -O3 -std=c++0x -ffast-math -pedantic -Werror -Wall -Wextra -Weffc++ -fno-default-inline -pipe -D_FILE_OFFSET_BITS=64 -D_XOPEN_SOURCE=500 -D_GNU_SOURCE
LIBS = -lm -lrt

all: $(executables)

schism: schism.o $(objects)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

-include depend

depend: $(source)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MM $(source) > depend

.PHONY: clean
clean:
	-rm -f $(executables) depend *.o *.rpo scheduling-algms/*.o
