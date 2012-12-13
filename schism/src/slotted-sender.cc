#include "slotted-sender.hh"
#include "packet.hh"
#include <stdlib.h>
#include <fstream>

SlottedSender::SlottedSender( int32_t flow_id, double probability ) :
	_flow_id( flow_id ),
	_probability( probability ),
	_tick( 0 ),
	_flow_queue( std::queue<Packet>() )
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
	double sample = ( double )random() / RAND_MAX;
	if ( sample < _probability )
	{
		Packet p(_flow_id, _tick);
		_flow_queue.push(p);
	}
}
