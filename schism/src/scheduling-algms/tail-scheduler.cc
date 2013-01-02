#include "tail-scheduler.hh"
#include <algorithm>

TailScheduler::TailScheduler() :
	Scheduler(),
	_flow_queues( std::vector<std::queue<Packet>> () ),
	_history( std::vector<std::vector<Packet>> () )
{}

void TailScheduler::add_sender( double weight )
{
	_flow_queues.push_back( std::queue<Packet>() );
	_history.push_back( std::vector<Packet>() );
	weight = weight + 1; /* dummy variable for TailScheduler */
}

Packet TailScheduler::get_next_packet()
{
	int64_t max_tail = -1;
	uint32_t max_arg  = _flow_queues.size();
	for ( uint32_t i=0; i < _flow_queues.size(); i++ ) {
		if ( _flow_queues.at( i ). empty() ) {
			continue;
		}
		int64_t current_tail = get_tail_delay( _history.at( i ), i );
		assert( current_tail >= 0 );
		if ( current_tail >= max_tail ) {
			max_tail = current_tail;
			max_arg  = i;
		}
	}
	return ( max_arg == _flow_queues.size() ) ? Packet( -1,-1) : dequeue( max_arg );
}

void TailScheduler::tick( uint64_t current_tick, std::vector<Packet> new_pkts )
{
	_tick = current_tick;
	std::for_each ( new_pkts.begin(), new_pkts.end(), [&] (Packet x) { enqueue( x ); } );
}

Packet TailScheduler::dequeue( uint32_t flow_id )
{
	assert( !_flow_queues.at( flow_id ).empty() );
	Packet to_send = _flow_queues.at( flow_id ).front();
	_flow_queues.at( flow_id ).pop();
	int64_t delay = _tick - to_send._tick;
	fprintf( stderr, "Sent packet out at %lu , from flow %u with delay %ld, queue size is %lu \n", _tick, flow_id, delay, _flow_queues.at( flow_id ).size() );
	to_send._delivered = _tick;
	to_send._delay     = delay;

	/* update history and purge old packets */
	_history.at( flow_id ).push_back( to_send );
	_history.at( flow_id ).erase( std::remove_if(_history.at( flow_id ).begin(), _history.at( flow_id ).end(),
	                                             [&] ( const Packet & x ) { return ( int64_t ) x._delivered < ( int64_t ) _tick - ( int64_t ) WINDOW_DURATION; } ),
	                              _history.at( flow_id ).end());

	return to_send;
}

void TailScheduler::enqueue( Packet p )
{
	uint32_t flow_id = p._flow_id;
	_flow_queues.at( flow_id ).push( p );
}

int64_t TailScheduler::get_tail_delay( std::vector<Packet> history, uint32_t flow_id )
{
	/* Estimate delays of packets in queue */
	QdelayEstimator estimator( _flow_queues.at( flow_id ), service_time() );

	/* Compute delays from history */
	std::vector<uint64_t> historic_delays( history.size() );
	std::transform( history.begin(), history.end(), historic_delays.begin(),
	                [&] (const Packet & p)
	                { return p._delivered - p._tick;} );

	/*Estimate both distributions */
	Distribution queue_dist ( estimator.estimate_delays( _tick ) );
	Distribution history_dist( historic_delays );

	/* Compose the two */
	Distribution composed = history_dist.compose( queue_dist );

	/* Get quantile */
	return composed.quantile( 0.95 );
}
