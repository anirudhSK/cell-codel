#ifndef PACKET_HH
#define PACKET_HH
#include<stdlib.h>
#include<stdint.h>

class Packet
{
public :
	uint32_t _flow_id;
	uint32_t _size;
	uint64_t _tick;

public :
	Packet( uint32_t flow_id, uint64_t _tick );
};

#endif
