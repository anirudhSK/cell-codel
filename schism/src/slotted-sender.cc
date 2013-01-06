#include "slotted-sender.hh"

SlottedSender::SlottedSender( int32_t flow_id, double rate, int seed, int batch_size ) :
	_flow_id( flow_id ),
	_rate( rate ),
	_tick( 0 ),
	_arrivals( Poisson( rate, seed ) ),
	_batch_size( Uniform( batch_size, seed ) ),
	_seed( seed ),
	_current_batch( 1 )
{
	fprintf( stderr, "Batch size is %d \n", batch_size );
}

std::vector<Packet> SlottedSender::tick( uint64_t current_tick )
{
	_tick=current_tick;
	if ( _tick % 1000 == 0 ) {
		_current_batch = _batch_size.sample() ;
		auto batch_rate = _rate/_current_batch;
		fprintf( stderr, "Tick %lu batch rate -> %f, batch size %d \n", _tick, batch_rate, _current_batch );
		_arrivals = Poisson( batch_rate, _seed );
	}
	return generate_packets();
}

std::vector<Packet> SlottedSender::generate_packets()
{
	std::vector<Packet> pkt_vector;
	const int num_packets = _arrivals.sample() * _current_batch ;
	int count = 0;
	while ( count < num_packets )
	{
		Packet p(_flow_id, _tick);
		pkt_vector.push_back( p );
		count++;
	}
	return pkt_vector;
}
