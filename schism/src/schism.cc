#include "slotted-link.hh"
#include "slotted-sender.hh"
#include "scheduler.hh"
#include "scheduling-algms/tail-scheduler.hh"
#include <assert.h>

int main( int argc, char* argv[] )
{
	assert (argc == 4);
	int seed  = atoi( argv[1] );
	int batch_size = atoi( argv[2] );
	float ingress_rate = atof( argv[3] );
	/* Pick a Scheduler */
	Scheduler * scheduler = new TailScheduler();
	
	/* Next sender */
	std::vector<SlottedSender *> sender_list;

	/* pick 500 senders */
	int N = 500;
	uint32_t num_ticks = 100000;
	int i = 0;
	fprintf( stderr, "Using N = %d  senders, ingress rate = %f, batch size %d, running for %u ticks \n", N, ingress_rate, batch_size, num_ticks);
	for ( i=0; i<N; i++ )
	{
		float rate = ingress_rate/N ;
		fprintf(stderr,"Using rate %f \n",rate);
		SlottedSender* next_sender = new SlottedSender( i, rate, seed, batch_size );
		sender_list.push_back( next_sender );
		scheduler->add_sender( 1.0 );
	}
	/* Create Link and attach scheduler */
	SlottedLink link( scheduler, seed );
	
	uint64_t current_tick=0;
	for ( current_tick=0; current_tick < num_ticks; current_tick++ )
	{
		link.tick(current_tick);
		std::vector<Packet> new_pkts;
		for (i=0; i<N; i++)
		{
			std::vector<Packet> pkts = sender_list.at(i)->tick( current_tick );
			new_pkts.insert( new_pkts.end(), pkts.begin(), pkts.end() );
		}
		scheduler->tick( current_tick, new_pkts );
	}
}
