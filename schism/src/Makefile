source = packet.cc slotted-link.cc slotted-sender.cc random.cc rr-scheduler.cc scheduling-algms/vclock-scheduler.cc
objects = packet.o slotted-link.o slotted-sender.o random.o rr-scheduler.o scheduling-algms/vclock-scheduler.o
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
	$(CXX) $(INCLUDES) -MM $(source) > depend

.PHONY: clean
clean:
	-rm -f $(executables) depend *.o *.rpo scheduling-algms/*.o
