#include "drr-scheduler.hh"
#include <algorithm>

DRRScheduler::DRRScheduler( bool t_codel_enabled ) :
	Scheduler(),
	_num_flows(0),
	_flow_queues( std::vector<std::queue<Packet>>() ),
	_flow_credits(  std::vector<double>() ),
	_flow_quantums( std::vector<double>() ),
	_active_list( std::queue<uint32_t>() ),
	_active_indicator( std::vector<bool>() ),
	_current_flow( 0 ),
	_codel_servo_bank( std::vector<CoDel> () ),
	_codel_enabled( t_codel_enabled )
{}

void DRRScheduler::add_sender( double weight )
{
	/* Add sender with specified weight */
	assert ( weight >= 1 );
	_flow_queues.push_back( std::queue<Packet>() );
	_flow_credits.push_back( 0 );
	_flow_quantums.push_back( weight );
	_active_indicator.push_back( false );
	if ( _codel_enabled ) {
		_codel_servo_bank.push_back( CoDel( &(_flow_queues.at( _num_flows )), _num_flows ) );
	}
	fprintf( stderr, "DRR: Adding flow %d with weight %f  \n", _num_flows, weight);
	_num_flows++;
}

void DRRScheduler::tick( uint64_t current_tick, std::vector<Packet> new_pkts )
{
	_tick = current_tick;
	std::for_each ( new_pkts.begin(), new_pkts.end(), [&] (Packet x) { enqueue( x ); } );
}

void DRRScheduler::enqueue( Packet p )
{
	uint32_t flow_id = p._flow_id;
	_flow_queues.at( flow_id ).push( p );
	if ( _active_indicator.at ( flow_id ) == false ) {
		_active_indicator.at( flow_id ) = true;
		_active_list.push( flow_id );
		_flow_credits.at( flow_id )=0;
	}
}

Packet DRRScheduler::dequeue( uint32_t flow_id )
{
	assert( !_flow_queues.at( flow_id ).empty() );
	if ( _codel_enabled ) {
		DelayedPacket packet = _codel_servo_bank.at( flow_id ).deque( _tick );
		if(packet.contents.size() > 0) {
			Packet to_send = packet.contents;
			int64_t delay = _tick - to_send._tick;
			fprintf( stderr, "seqnum %lu delay %ld flowid %u tick %lu queue %lu\n", to_send._seq_num, delay, flow_id, _tick, _flow_queues.at( flow_id ).size() );
			return to_send;
		} else {
			return Packet( -1, -1 );
		}
	}
	Packet to_send = _flow_queues.at( flow_id ).front();
	_flow_queues.at( flow_id ).pop();
	int64_t delay = _tick - to_send._tick;
	fprintf( stderr, "seqnum %lu delay %ld flowid %u tick %lu queue %lu\n", to_send._seq_num, delay, flow_id, _tick, _flow_queues.at( flow_id ).size() );
	return to_send;
}

Packet DRRScheduler::get_next_packet()
{
	uint32_t pkt_size = 1; /* for now, all packets are of unit size */
	Packet p(-1,-1);
	if ( _active_list.empty() ) {
		return p;
	}
	_current_flow = _active_list.front();
	if ( _flow_credits.at ( _current_flow ) < pkt_size ) { /* do not add until you deplete credits */
		_flow_credits.at( _current_flow ) += _flow_quantums.at( _current_flow );
	}
	assert ( !_flow_queues.at( _current_flow ).empty() ) ;
	assert ( pkt_size <= _flow_credits.at( _current_flow ) ) ;
	_flow_credits.at( _current_flow ) -= pkt_size;
	p=dequeue( _current_flow );
	if ( _flow_queues.at( _current_flow ).empty() ) {
		_flow_credits.at( _current_flow ) = 0;
		_active_list.pop();
		_active_indicator.at( _current_flow )=false;
	}
	else if ( _flow_credits.at( _current_flow ) < pkt_size ) {
		_active_list.pop();
		_active_list.push( _current_flow );
	}
	return p;
}
