#include <util/random_engine.h>
#include <chrono>

RandomEngine::RandomEngine()
{
	auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	this->randomGenerator = std::mt19937(static_cast<uint32_t>(seed));
}

RandomEngine& RandomEngine::GetInstance()
{
	static RandomEngine instance;
	return instance;
}
