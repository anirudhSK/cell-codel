#include "slotted-sender.hh"
#include "packet.hh"
#include <stdlib.h>
#include <fstream>

SlottedSender::SlottedSender( int32_t flow_id, double rate ) :
	_flow_id( flow_id ),
	_rate( rate ),
	_tick( 0 ),
	_flow_queue( std::queue<Packet>() ),
	_arrivals( Poisson ( rate ) )
{}

void SlottedSender::tick( uint64_t current_tick )
{
	_tick=current_tick;
	receive_packet();
}

void SlottedSender::send_packet()
{
	Packet to_send = _flow_queue.front();
	_flow_queue.pop();
	int64_t delay = _tick - to_send.get_tick();
	fprintf( stderr, "Sent packet out at %lu , from flow %u with delay %ld, queue size is %lu \n", _tick, _flow_id, delay, _flow_queue.size() );
}

void SlottedSender::receive_packet()
{
	const int num_packets = _arrivals.sample();
	int count = 0;
	while ( count < num_packets )
	{
		Packet p(_flow_id, _tick);
		_flow_queue.push(p);
		count++;
	}
}
