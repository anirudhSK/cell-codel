#ifndef TAIL_SCHEDULER_HH
#define TAIL_SCHEDULER_HH

#include "../slotted-sender.hh"
#include "../packet.hh"
#include "../scheduler.hh"

/* Tail Scheduler (Largest 95th percentile first) */
class TailScheduler : public Scheduler
{
private :
	/* Tail Scheduler structures */
	std::vector<std::queue<Packet>> _flow_queues;
	std::vector<std::vector<Packet>> _history;
	static const uint32_t WINDOW_DURATION = 5000 ;

public :
	TailScheduler();
	
	void add_sender( double weight );
	
	Packet get_next_packet();
	
	void tick( uint64_t current_tick, std::vector<Packet> new_pkts );
	
	Packet dequeue( uint32_t flow_id );
	
	void enqueue( Packet p);
	
	int64_t get_tail_delay( std::vector<Packet> history );
};

#endif