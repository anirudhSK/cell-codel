#ifndef SLOTTED_LINK_HH
#define SLOTTED_LINK

#include <list>
#include "slotted-sender.hh"

/* Slotted work conserving link; sends one packet per time slot */
class SlottedLink
{
private :
	/* Required for any scheduling algorithm */
	/* References to senders */
	uint32_t _num_flows;
	std::vector<SlottedSender const *> _sender_list;
	uint64_t _tick;

	/* Required for RR scheduling */
	uint32_t _current_flow;

public :
	SlottedLink( void );
	
	void tick( uint64_t current_tick );
	
	void add_sender( SlottedSender const * sender );
	
};

#endif
