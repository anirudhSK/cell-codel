#include "slotted-link.hh"
#include<fstream>

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
	SlottedSender* _current_sender = (SlottedSender* ) _sender_list.at( _current_flow );
	uint32_t rr_index = 0;
	/* Work conserving */
	while ( ( !_current_sender->is_backlogged() )  && ( rr_index < _num_flows ) ) {
		_current_flow=( _current_flow + 1 ) % _num_flows;
		_current_sender= (SlottedSender *) _sender_list.at( _current_flow );
		rr_index++;
	}
	if( rr_index == _num_flows ) {
//		fprintf(stderr, "Wasted tick @ %lu since all queues were empty \n", _tick);
	} else {
		_current_sender->send_packet();
		_current_flow=( _current_flow + 1 ) % _num_flows;
	}
	
	_tick=current_tick;
}
