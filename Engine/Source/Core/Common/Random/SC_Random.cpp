#include "SC_Random.h"

#include <random>

static std::mt19937& GetGenerator()
{
	static std::random_device device;
	static std::mt19937 generator(device());
	return generator;
}

int32 SC_Random::Range(int32 aMin, int32 aMax)
{
	std::uniform_int_distribution<int32> distribution(aMin, aMax);
	return distribution(GetGenerator());
}

uint32 SC_Random::Range(uint32 aMin, uint32 aMax)
{
	std::uniform_int_distribution<uint32> distribution(aMin, aMax);
	return distribution(GetGenerator());
}

uint64 SC_Random::Range(uint64 aMin, uint64 aMax)
{
	std::uniform_int_distribution<uint64> distribution(aMin, aMax);
	return distribution(GetGenerator());
}

float SC_Random::Range(float aMin, float aMax)
{
	std::uniform_real_distribution<float> distribution(aMin, aMax);
	return distribution(GetGenerator());
}
