#pragma once 

class RandomNumberGenerator;

extern RandomNumberGenerator* g_rng;

class RandomNumberGenerator
{
	public:
		
	public:
		int RollRandomIntLessThan( int maxNotInclusive );
		int RollRandomIntInRange( int minInclusive, int maxInclusive );
		float RollRandomFloatZeroToOne();
		float RollRandomFloatInRange( float minInclusive, float maxInclusive );

	private:

};