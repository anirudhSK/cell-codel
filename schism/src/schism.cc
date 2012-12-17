#include "slotted-link.hh"
#include "slotted-sender.hh"
#include <stdlib.h>
#include <assert.h>
#include <fstream>

int main( int argc, char* argv[] )
{
	assert (argc == 3);
	int seed  = atoi( argv[1] );
	uint32_t x = atoi (argv[2]);
	/* make it deterministic */
	srand( seed );

	/* Link */
	SlottedLink link;

	/* Next sender */
	std::vector<SlottedSender *> sender_list;

	/* pick 10 senders */
	int N = 2;
	int i = 0;
	fprintf(stderr,"seed is %d \n",seed);
	for ( i=0; i<N; i++ )
	{
		uint32_t weight = ( i==0 ) ? x : 99 -x;
		fprintf(stderr,"Using rate %f \n",(float)weight/100.0);
		SlottedSender* next_sender = new SlottedSender( i, (float)weight/100.0, seed );
		sender_list.push_back( next_sender );
		link.add_sender( next_sender, weight );
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
