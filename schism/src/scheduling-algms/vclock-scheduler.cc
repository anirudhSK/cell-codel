#include "vclock-scheduler.hh"
#include <algorithm>

VClockScheduler::VClockScheduler() :
	_num_flows(0),
	_flow_queues( std::vector<std::queue<TaggedPacket>> () ),
	_intervals( std::vector<double> () ),
	_avc( std::vector<double> () ),
	_tick( 0 )
{}

void VClockScheduler::add_sender( double avg_interval )
{
	/* Add sender with specified weight */
	assert ( avg_interval >= 1 );
	_flow_queues.push_back( std::queue<TaggedPacket>() );
	_intervals.push_back( avg_interval );
	_avc.push_back( _tick );
	fprintf( stderr, "Adding flow %d with avg_interval %f  \n", _num_flows, avg_interval);
	_num_flows++;
}

void VClockScheduler::tick( uint64_t current_tick, std::vector<Packet> new_pkts )
{
	_tick = current_tick;
	std::for_each ( new_pkts.begin(), new_pkts.end(), [&] (Packet x) { enqueue( x ); } );
}

void VClockScheduler::enqueue( Packet p )
{
	uint32_t flow_id = p._flow_id;
	_avc.at( flow_id ) = std::max( _avc.at( flow_id ), (double) _tick );
	_avc.at( flow_id ) += _intervals.at( flow_id );
	TaggedPacket tagged_pkt( p, _avc.at( flow_id) );
	_flow_queues.at( flow_id ).push( tagged_pkt );
}

Packet VClockScheduler::dequeue( uint32_t flow_id )
{
	assert( !_flow_queues.at( flow_id ).empty() );
	Packet to_send = std::get<0>( _flow_queues.at( flow_id ).front() );
	_flow_queues.at( flow_id ).pop();
	int64_t delay = _tick - to_send._tick;
	fprintf( stderr, "Sent packet out at %lu , from flow %u with delay %ld, queue size is %lu \n", _tick, flow_id, delay, _flow_queues.at( flow_id ).size() );
	return to_send;
}

Packet VClockScheduler::get_next_packet()
{
	std::vector<double> hol_tags( _num_flows );
	/* get head of line tags on each flow queue if non-empty , then take the min. among them */
	std::transform( _flow_queues.begin(), _flow_queues.end(), hol_tags.begin(), \
                [&] (std::queue<TaggedPacket> x) { return ( x.empty() ? 1.0e9 : std::get<1>(x.front()) ) ;} );
	uint32_t arg_min = std::distance( hol_tags.begin(), std::min_element( hol_tags.begin(), hol_tags.end() ) );
	return ( hol_tags.at( arg_min ) >= 1.0e8 ) ? Packet(-1,-1) : dequeue( arg_min ) ;
}
