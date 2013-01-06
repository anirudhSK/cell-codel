#ifndef SLOTTED_SENDER_HH
#define SLOTTED_SENDER_HH

#include <queue>
#include "packet.hh"
#include "poisson.hh"
#include "uniform.hh"

class SlottedSender
{
private :
	uint32_t _flow_id;
	double _rate;
	uint64_t _tick;
	Poisson _arrivals;
	Uniform _batch_size;
	int _seed;
	int _current_batch;
	
public :
	SlottedSender( int32_t flow_id, double rate, int seed, int batch_size ) ;
	
	std::vector<Packet> tick( uint64_t current_tick );
	
	std::vector<Packet> generate_packets();
};

#endif
