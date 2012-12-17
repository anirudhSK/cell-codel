#ifndef SLOTTED_LINK_HH
#define SLOTTED_LINK

#include <list>
#include "slotted-sender.hh"
#include "rr-scheduler.hh"

/* Slotted work conserving link; sends one packet per time slot */
class SlottedLink
{
private :
	uint64_t _tick;

	RRScheduler * _scheduler;

public :
	SlottedLink( RRScheduler * scheduler );
	
	void tick( uint64_t current_tick );
	
	/* get packet delivery opportunities for this slot (name courtesy Keith) */
	uint32_t get_pdos() { return 1; /* Change this for a time varying link */}
};

#endif
