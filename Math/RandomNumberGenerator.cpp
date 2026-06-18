#include "Engine/Math/RandomNumberGenerator.hpp"
#include <stdlib.h>

RandomNumberGenerator* g_rng = nullptr;


int RandomNumberGenerator::RollRandomIntLessThan(int maxNotInclusive)
{
	return rand() % maxNotInclusive;
}
int RandomNumberGenerator::RollRandomIntInRange(int minInclusive, int maxInclusive)
{
	
	int range = maxInclusive - minInclusive + 1;
	return (rand() % range) + minInclusive;
;
}
float RandomNumberGenerator::RollRandomFloatZeroToOne()
{
	return static_cast<float>(rand()) / static_cast<float>( RAND_MAX);
}
float RandomNumberGenerator::RollRandomFloatInRange(float minInclusive, float maxInclusive)
{
	float t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX); // in [0,1]
	return minInclusive + t * (maxInclusive - minInclusive);
}
