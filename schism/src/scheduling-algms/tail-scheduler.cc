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
	fprintf( stderr, "TailScheduler: Adding sender \n");
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
	/* Ensure scheduler is work conserving */
	if( std::find_if( _flow_queues.begin(), _flow_queues.end(), [&] (const std::queue<Packet> & flow) { return !flow.empty();} ) != _flow_queues.end() ) {
		assert (max_arg != _flow_queues.size());
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
	fprintf( stderr, "seqnum %lu delay %ld flowid %u tick %lu queue %lu\n", to_send._seq_num, delay, flow_id, _tick, _flow_queues.at( flow_id ).size() );
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
	QdelayEstimator estimator( _flow_queues.at( flow_id ), service_time( flow_id ) );

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

uint64_t TailScheduler::service_time( uint32_t flow_id )
{
	/* Get mean inter departure time for all packets in history */
	auto flow_hist = _history.at( flow_id );
	uint64_t sum = 0;
	for ( uint32_t i=1; i < flow_hist.size(); i++ ) {
		sum += flow_hist.at( i )._delivered - flow_hist.at( i-1 )._delivered;
	}
	return ( flow_hist.size() >=2 ) ? sum/(flow_hist.size() - 1) : 1 ;
}
