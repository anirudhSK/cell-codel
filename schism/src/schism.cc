#include "slotted-link.hh"
#include "slotted-sender.hh"
#include <stdlib.h>
#include <assert.h>
#include <fstream>

int main( int argc, char* argv[] )
{
	assert (argc == 2);
	int seed = atoi( argv[1] );
	/* make it deterministic */
	srand( seed );

	/* Link */
	SlottedLink link;
		
	/* Next sender */
	std::vector<SlottedSender *> sender_list;

	/* pick 10 senders */
	int N = 10;
	int i=0;
	for (i=0; i<N; i++)
	{
		//float rate=(i*1.0)/45.0;
		float rate=0.1;
		fprintf(stderr,"Using rate %f \n",rate);
		SlottedSender* next_sender = new SlottedSender (i, rate);
		sender_list.push_back( next_sender );
		link.add_sender( next_sender );
	}
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
