#ifndef RR_SCHEDULER_HH
#define RR_SCHEDULER_HH

#include "slotted-sender.hh"
#include "packet.hh"

/* Weighted Round Robin Scheduler */
class RRScheduler
{
private :
	/* References to senders */
	uint32_t _num_flows;
	std::vector<SlottedSender const *> _sender_list;
	std::vector<uint32_t> _slot_schedule;

	/* Book-keeping for RR */
	uint32_t _current_slot;
	uint32_t _num_slots;

public :
	RRScheduler();
	
	void add_sender( SlottedSender const * sender, uint32_t weight );
	
	SlottedSender * get_sender_for_slot( uint32_t slot );
	
	Packet get_next_packet();
};

#endif
