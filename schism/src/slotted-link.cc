#include "slotted-link.hh"
#include <fstream>
#include <assert.h>

SlottedLink::SlottedLink( RRScheduler* scheduler) :
	_tick( 0 ),
	_scheduler( scheduler )
{}


void SlottedLink::tick( uint64_t current_tick )
{
	uint32_t num_pdos = get_pdos();
	for ( uint32_t i=0; i < num_pdos; i++ ) {
		_scheduler->get_next_packet();
		/* TODO : Do something if needed with ret. value of get_next_packet */
	}
	
	_tick=current_tick;
}
