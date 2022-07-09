#ifndef RANDOM_ENGINE_H
#define RANDOM_ENGINE_H

#include <random>

class RandomEngine
{
private:
	std::mt19937 randomGenerator;
private:
	RandomEngine();
public:
	RandomEngine(const RandomEngine& other) = delete;
	RandomEngine(RandomEngine&& temp) noexcept = delete;
	~RandomEngine() = default;

	// Returns random number between the specified min and max values.
	template<typename T> T GenerateRandom(T min, T max);

	// Returns random number between the specified min and max values.
	template<> int GenerateRandom<int>(int min, int max);

	// Returns random number between the specified min and max values.
	template<> float GenerateRandom<float>(float min, float max);

	// Returns random number between the specified min and max values.
	template<> double GenerateRandom<double>(double min, double max);

	// Returns singleton instance object of this class.
	static RandomEngine& GetInstance();
};

#include <util/random_engine.tpp>

#endif