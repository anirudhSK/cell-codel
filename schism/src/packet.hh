#ifndef PACKET_HH
#define PACKET_HH

#include<stdint.h>

class Packet
{
public :
	uint64_t _seq_num;
	uint32_t _flow_id;
	uint32_t _size;
	uint64_t _tick;
	uint64_t _delivered;
	int64_t _delay;
	static uint64_t current_seq_num;
	static const uint64_t PACKET_SIZE = 1;

public :
	Packet( uint32_t flow_id, uint64_t _tick );

	uint32_t size() { return ( _flow_id == (uint32_t) -1 ) ? 0 : PACKET_SIZE ; }
};

#endif
