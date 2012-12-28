#ifndef SCHEDULER_HH
#define SCHEDULER_HH

#include "slotted-sender.hh"
#include "packet.hh"

/* Abstract Scheduler */
class Scheduler
{
protected :
	uint64_t _tick;
public :
	Scheduler() : _tick( 0 ) {};
	
	virtual Packet get_next_packet() =  0;
	
	virtual void tick( uint64_t current_tick, std::vector<Packet> new_pkts ) = 0;
	
	virtual void enqueue( Packet p ) = 0;
	
	virtual void add_sender( double weight_coefficient ) { weight_coefficient += 0; };
	
	virtual ~Scheduler() {};
};

#endif
