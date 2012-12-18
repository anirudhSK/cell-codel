#include "slotted-sender.hh"
#include <stdlib.h>
#include <fstream>
#include "assert.h"

SlottedSender::SlottedSender( int32_t flow_id, double rate, int seed ) :
	_flow_id( flow_id ),
	_rate( rate ),
	_tick( 0 ),
	_flow_queue( std::queue<Packet>() ),
	_arrivals( Poisson ( rate, seed ) )
{}

std::vector<Packet> SlottedSender::tick( uint64_t current_tick )
{
	_tick=current_tick;
	return generate_packets();
}

std::vector<Packet> SlottedSender::generate_packets()
{
	std::vector<Packet> pkt_vector;
	const int num_packets = _arrivals.sample();
	int count = 0;
	while ( count < num_packets )
	{
		Packet p(_flow_id, _tick);
		pkt_vector.push_back( p );
		count++;
	}
	return pkt_vector;
}
