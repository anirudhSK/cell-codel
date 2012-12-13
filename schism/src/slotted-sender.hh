#ifndef SLOTTED_SENDER_HH
#define SLOTTED_SENDER_HH

#include <queue>
#include "packet.hh"

class SlottedSender
{
private :
	uint32_t _flow_id;
	double _probability;
	uint64_t _tick;
	std::queue<Packet> _flow_queue;
	
public :
	SlottedSender( int32_t flow_id, double probability ) ;
	
	void tick( uint64_t current_tick );
	
	void receive_packet();

	void send_packet();

	bool is_backlogged() { return !_flow_queue.empty(); }
};

#endif
