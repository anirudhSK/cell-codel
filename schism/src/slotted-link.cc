#include "slotted-link.hh"

SlottedLink::SlottedLink( Scheduler* scheduler, int seed ) :
	_tick( 0 ),
	_scheduler( scheduler ),
	_pdos( Poisson ( 1, seed ) )
{}

void SlottedLink::tick( uint64_t current_tick )
{
	uint32_t num_pdos = 1;
	for ( uint32_t i=0; i < num_pdos; i++ ) {
		_scheduler->get_next_packet();
		/* TODO : Do something if needed with ret. value of get_next_packet */
	}
	
	_tick=current_tick;
}
