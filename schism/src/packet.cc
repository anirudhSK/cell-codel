#include "packet.hh"

uint64_t Packet::current_seq_num = 0;

Packet::Packet( uint32_t flow_id, uint64_t tick) :
	_seq_num( ( flow_id == (uint32_t) -1 ) ? current_seq_num : ++current_seq_num ),
	_flow_id( flow_id ),
	_size( 1 ),
	_tick( tick ),
	_delivered( (uint64_t)-1 ),
	_delay( -1 )
{}
