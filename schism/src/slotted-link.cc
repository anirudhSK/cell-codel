#include "slotted-link.hh"
#include<fstream>
#include<assert.h>
SlottedLink::SlottedLink() :
	_num_flows( 0 ),
	_sender_list( std::vector<SlottedSender const *>() ),
	_slot_schedule( std::vector<uint32_t>() ),
	_tick( 0 ),
	_current_slot( 0 ),
	_num_slots( 0 )
{}

void SlottedLink::add_sender( SlottedSender const * sender, uint32_t weight )
{
	/* Add sender with specified weight */
	assert ( weight >= 1 );
	_sender_list.push_back( sender );
	_num_slots += weight ;
	_slot_schedule.push_back( _num_slots );
	_num_flows++;
}

SlottedSender * SlottedLink::get_sender_for_slot( uint32_t slot )
{
	for ( uint32_t i=0 ; i < _num_flows; i++ ) {
		if( slot < _slot_schedule.at( i ) ) {
			return (SlottedSender *)_sender_list.at ( i );
		}
	}
	assert( false );
}
void SlottedLink::tick( uint64_t current_tick )
{
	/* Move on to the next flow */
	uint32_t old_slot = _current_slot;
	_current_slot=( old_slot + 1 ) % _num_slots ;
	SlottedSender * _current_sender = get_sender_for_slot( _current_slot );
	/* Check if work needs to be done, since RR is work conserving */
	uint32_t rr_index = 0;
	while ( ( !_current_sender->is_backlogged() )  && ( rr_index < _num_slots ) ) {
		_current_slot   = ( _current_slot + 1 ) % _num_slots;
		_current_sender = get_sender_for_slot( _current_slot );
		rr_index++;
		assert( _current_slot == ( old_slot + 1 + rr_index ) % _num_slots );
	}
	if( rr_index == _num_slots ) {
		assert ( _current_slot == ( old_slot + 1 ) % _num_slots );
		/* back to square one */
//		fprintf(stderr, "Wasted tick @ %lu since all queues were empty \n", _tick);
	} else {
		_current_sender->send_packet();
	}
	
	_tick=current_tick;
}
