#ifndef FCFS_SCHEDULER_HH
#define FCFS_SCHEDULER_HH

#include "../packet.hh"
#include "../scheduler.hh"
#include <queue>

/* FCFS Scheduler */
class FcfsScheduler : public Scheduler
{
private :
	std::queue<Packet> _flow_queue;

public :
	FcfsScheduler();
	
	Packet get_next_packet();
	
	void tick( uint64_t current_tick, std::vector<Packet> new_pkts );
	
	Packet dequeue();
	
	void enqueue( Packet p );
};

#endif
