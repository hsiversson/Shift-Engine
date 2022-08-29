#pragma once

class SC_Random
{
public:
	static int32 Range(int32 aMin, int32 aMax);
	static uint32 Range(uint32 aMin, uint32 aMax);
	static uint64 Range(uint64 aMin, uint64 aMax);
	static float Range(float aMin, float aMax);

	template<class T>
	static T ZeroToOne()
	{
		return Range(T(0), T(1));
	}
};

