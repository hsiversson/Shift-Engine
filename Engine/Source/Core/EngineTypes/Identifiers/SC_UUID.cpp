#include "SC_UUID.h"
#include <random>

static std::random_device gRandomDevice;
static std::mt19937_64 gRandomEngine(gRandomDevice());
static std::uniform_int_distribution<uint64> gUniformDistribution0;
static std::uniform_int_distribution<uint64> gUniformDistribution1;

SC_UUID::SC_UUID()
	: mUUID{ gUniformDistribution0(gRandomEngine), gUniformDistribution1(gRandomEngine) }
{

}

SC_UUID::~SC_UUID()
{

}

bool SC_UUID::operator==(const SC_UUID& aOther)
{
	if (mUUID.mLowComp == aOther.mUUID.mLowComp && mUUID.mHighComp == aOther.mUUID.mHighComp)
		return true;

	return false;
}

bool SC_UUID::operator!=(const SC_UUID& aOther)
{
	return !(*this == aOther);
}