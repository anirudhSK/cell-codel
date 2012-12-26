#ifndef RR_SCHEDULER_HH
#define RR_SCHEDULER_HH

#include "scheduler.hh"
#include "slotted-sender.hh"
#include "packet.hh"

/* Weighted Round Robin Scheduler */
class RRScheduler : public Scheduler
{
private :
	/* References to senders */
	uint32_t _num_flows;
	std::vector<std::queue<Packet>> _flow_queues;
	std::vector<uint32_t> _slot_schedule;

	/* Book-keeping for RR */
	uint32_t _current_slot;
	uint32_t _num_slots;

public :
	RRScheduler();
	
	void add_sender( uint32_t weight );
	
	uint32_t get_sender_for_slot( uint32_t slot );
	
	Packet get_next_packet();
	
	void tick( uint64_t current_tick, std::vector<Packet> new_pkts );
	
	Packet dequeue( uint32_t flow_id );
	
	void enqueue( Packet p);
};

#endif
