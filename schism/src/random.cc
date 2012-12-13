#include <time.h>

#include "random.hh"

boost::random::mt19937 & get_generator( int seed )
{
	static boost::random::mt19937 generator( seed );
	return generator;
}
