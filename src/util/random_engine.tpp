#include <util/random_engine.h>

template<typename T> T RandomEngine::GenerateRandom(T min, T max)
{
	static_assert(false);
}

template<> int RandomEngine::GenerateRandom(int min, int max)
{
	std::uniform_int_distribution<int> random_distributor(min, max);
	return random_distributor(this->randomGenerator);
}

template<> float RandomEngine::GenerateRandom(float min, float max)
{
	std::uniform_real_distribution<float> random_distributor(min, max);
	return random_distributor(this->randomGenerator);
}

template<> double RandomEngine::GenerateRandom(double min, double max)
{
	std::uniform_real_distribution<double> random_distributor(min, max);
	return random_distributor(this->randomGenerator);
}