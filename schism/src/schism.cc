#include "slotted-link.hh"
#include "slotted-sender.hh"
#include "scheduler.hh"
#include "scheduling-algms/rr-scheduler.hh"
#include <assert.h>

int main( int argc, char* argv[] )
{
	assert (argc == 3);
	int seed  = atoi( argv[1] );
	bool skew = ( std::string(argv[2]) == "skew" ) ;
	
	/* Pick a Scheduler */
	Scheduler * scheduler = new RRScheduler();
	
	/* Next sender */
	std::vector<SlottedSender *> sender_list;

	/* pick 10 senders */
	int N = 10;
	int batch_size = 1;
	int i = 0;
	for ( i=0; i<N; i++ )
	{
		float rate = skew ?  (i*0.99/45.0) + 1e-30 :  0.099 + 1e-30;
		fprintf(stderr,"Using rate %f \n",rate);
		SlottedSender* next_sender = new SlottedSender( i, rate, seed, batch_size );
		sender_list.push_back( next_sender );
		scheduler->add_sender( 1.0 );
	}
	/* Create Link and attach scheduler */
	SlottedLink link( scheduler, seed );
	
	uint64_t current_tick=0;
	for ( current_tick=0; current_tick < 1000000; current_tick++ )
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
