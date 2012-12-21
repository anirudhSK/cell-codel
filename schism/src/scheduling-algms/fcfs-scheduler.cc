#include "fcfs-scheduler.hh"
#include <algorithm>
#include <assert.h>

FcfsScheduler::FcfsScheduler() :
	_flow_queue( std::queue<Packet> () ),
	_tick( 0 )
{}

void FcfsScheduler::tick( uint64_t current_tick, std::vector<Packet> new_pkts )
{
	_tick = current_tick;
	std::for_each ( new_pkts.begin(), new_pkts.end(), [&] (Packet x) { enqueue( x ); } );
}

void FcfsScheduler::enqueue( Packet p )
{
	_flow_queue.push( p );
}

Packet FcfsScheduler::dequeue()
{
	assert( !_flow_queue.empty() );
	Packet to_send = _flow_queue.front();
	_flow_queue.pop();
	int64_t delay = _tick - to_send._tick;
	fprintf( stderr, "Sent packet out at %lu , from flow %u with delay %ld, queue size is %lu \n", _tick, to_send._flow_id, delay, _flow_queue.size() );
	return to_send;
}

Packet FcfsScheduler::get_next_packet()
{
	if( _flow_queue.empty() ) {
		//fprintf(stderr, "Wasted tick @ %lu since all queues were empty \n", _tick);
		return *(new Packet(-1,-1));
	} else {
		return dequeue();
	}
}
