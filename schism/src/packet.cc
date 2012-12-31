#include "packet.hh"
Packet::Packet( uint32_t flow_id, uint64_t tick) :
	_flow_id( flow_id ),
	_size( 1 ),
	_tick( tick ),
	_delivered( (uint64_t)-1 ),
	_delay( -1 )
{}
