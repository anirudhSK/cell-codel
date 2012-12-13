#include "slotted-link.hh"
#include<fstream>
#include<assert.h>
SlottedLink::SlottedLink() :
	_num_flows( 0 ),
	_sender_list( std::vector<SlottedSender const *>() ),
	_tick( 0 ),
	_current_flow( 0 )
{}

void SlottedLink::add_sender( SlottedSender const * sender )
{
	_sender_list.push_back( sender );
	_num_flows++;
}

void SlottedLink::tick( uint64_t current_tick )
{
	/* Move on to the next flow */
	uint32_t old_flow = _current_flow;
	_current_flow=( old_flow + 1 ) % _num_flows ;
	SlottedSender* _current_sender = (SlottedSender* ) _sender_list.at( _current_flow );
	/* Check if work needs to be done, since RR is work conserving */
	uint32_t rr_index = 0;
	while ( ( !_current_sender->is_backlogged() )  && ( rr_index < _num_flows ) ) {
		_current_flow=( _current_flow + 1 ) % _num_flows;
		_current_sender= (SlottedSender *) _sender_list.at( _current_flow );
		rr_index++;
		assert( _current_flow == ( old_flow + 1 + rr_index ) % _num_flows );
	}
	if( rr_index == _num_flows ) {
		assert ( _current_flow == ( old_flow + 1 ) % _num_flows );
		/* back to square one */
//		fprintf(stderr, "Wasted tick @ %lu since all queues were empty \n", _tick);
	} else {
		_current_sender->send_packet();
	}
	
	_tick=current_tick;
}
