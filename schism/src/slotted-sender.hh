#ifndef SLOTTED_SENDER_HH
#define SLOTTED_SENDER_HH

#include <queue>
#include "packet.hh"
#include "poisson.hh"

class SlottedSender
{
private :
	uint32_t _flow_id;
	double _rate;
	uint64_t _tick;
	std::queue<Packet> _flow_queue;
	Poisson _arrivals;
	
public :
	SlottedSender( int32_t flow_id, double rate, int seed ) ;
	
	std::vector<Packet> tick( uint64_t current_tick );
	
	std::vector<Packet> generate_packets();
};

#endif
