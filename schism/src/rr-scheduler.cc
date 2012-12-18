#include "rr-scheduler.hh"
#include <algorithm>

RRScheduler::RRScheduler() :
	_num_flows(0),
	_flow_queues( std::vector<std::queue<Packet>> () ),
	_slot_schedule( std::vector<uint32_t>() ),
	_current_slot( 0 ),
	_num_slots( 0 ),
	_tick( 0 )
{}

void RRScheduler::add_sender( uint32_t weight )
{
	/* Add sender with specified weight */
	assert ( weight >= 1 );
	_flow_queues.push_back( std::queue<Packet>() );
	_num_slots += weight ;
	_slot_schedule.push_back( _num_slots );
	fprintf( stderr, "Adding flow %d with weight %d  \n", _num_flows, weight);
	_num_flows++;
}

void RRScheduler::tick( uint64_t current_tick, std::vector<Packet> new_pkts )
{
	_tick = current_tick;
	std::for_each ( new_pkts.begin(), new_pkts.end(), [&] (Packet x) { enqueue( x ); } );
}

void RRScheduler::enqueue( Packet p )
{
	uint32_t flow_id = p._flow_id;
	_flow_queues.at( flow_id ).push( p );
}

Packet RRScheduler::dequeue( uint32_t flow_id )
{
	assert( !_flow_queues.at( flow_id ).empty() );
	Packet to_send = _flow_queues.at( flow_id ).front();
	_flow_queues.at( flow_id ).pop();
	int64_t delay = _tick - to_send._tick;
	fprintf( stderr, "Sent packet out at %lu , from flow %u with delay %ld, queue size is %lu \n", _tick, flow_id, delay, _flow_queues.at( flow_id ).size() );
	return to_send;
}

uint32_t RRScheduler::get_sender_for_slot( uint32_t slot )
{
	for ( uint32_t i=0 ; i < _num_flows; i++ ) {
		if( slot < _slot_schedule.at( i ) ) {
			return i;
		}
	}
	assert( false );
}

Packet RRScheduler::get_next_packet()
{
	/* Move on to the next flow */
	uint32_t old_slot = _current_slot;
	_current_slot=( old_slot + 1 ) % _num_slots ;
	uint32_t  current_sender = get_sender_for_slot( _current_slot );
	/* Check if work needs to be done, since RR is work conserving */
	uint32_t rr_index = 0;
	while ( ( _flow_queues.at( current_sender ).empty() )  && ( rr_index < _num_slots ) ) {
		_current_slot   = ( _current_slot + 1 ) % _num_slots;
		current_sender = get_sender_for_slot( _current_slot );
		rr_index++;
		assert( _current_slot == ( old_slot + 1 + rr_index ) % _num_slots );
	}
	if( rr_index == _num_slots ) {
		assert ( _current_slot == ( old_slot + 1 ) % _num_slots );
		/* back to square one */
		//fprintf(stderr, "Wasted tick @ %lu since all queues were empty \n", _tick);
		return *(new Packet(-1,-1));
	} else {
		return dequeue( current_sender );
	}
}
