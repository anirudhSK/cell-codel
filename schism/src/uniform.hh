#ifndef UNIFORM_HH
#define UNIFORM_HH

#include <boost/random/uniform_int_distribution.hpp>

#include "random.hh"

class Uniform
{
	private:
		boost::random::uniform_int_distribution<> distribution;
		int _seed;

	public:
		Uniform( int u_bound, int seed ) : distribution( 1, u_bound ) , _seed(seed) {}
		int sample( void ) { return distribution( get_generator( _seed ) ); }
};

#endif
