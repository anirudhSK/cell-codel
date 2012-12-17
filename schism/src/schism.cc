#include "slotted-link.hh"
#include "slotted-sender.hh"
#include "rr-scheduler.hh"
#include <stdlib.h>
#include <assert.h>
#include <fstream>

int main( int argc, char* argv[] )
{
	assert (argc == 3);
	int seed  = atoi( argv[1] );
	bool skew = ( std::string(argv[2]) == "skew" ) ;
	
	/* RR Scheduler */
	RRScheduler * rr_scheduler = new RRScheduler();
	
	/* Next sender */
	std::vector<SlottedSender *> sender_list;

	/* pick 10 senders */
	int N = 10;
	int i = 0;
	for ( i=0; i<N; i++ )
	{
		float rate = skew ?  (i*0.99/45.0) + 1e-30 :  0.099 + 1e-30;
		fprintf(stderr,"Using rate %f \n",rate);
		SlottedSender* next_sender = new SlottedSender( i, rate, seed );
		sender_list.push_back( next_sender );
		rr_scheduler->add_sender( next_sender, 1 );
	}
	/* Create Link and attach scheduler */
	SlottedLink link( rr_scheduler );
	
	uint64_t current_tick=0;
	for ( current_tick=0; current_tick < 1000000; current_tick++ )
	{
		link.tick(current_tick);
		for (i=0; i<N; i++)
		{
			sender_list.at(i)->tick(current_tick);
		}
	}
}
