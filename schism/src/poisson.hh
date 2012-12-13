#ifndef POISSON_HH
#define POISSON_HH

#include <boost/random/poisson_distribution.hpp>

#include "random.hh"

class Poisson
{
	private:
		boost::random::poisson_distribution<> distribution;
		int _seed;

	public:
		Poisson( double rate, int seed ) : distribution( rate ) , _seed(seed) {}
		int sample( void ) { return distribution( get_generator( _seed ) ); }
};

#endif
