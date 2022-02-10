#pragma once
#include <chrono>

class SC_Time
{
public:
	static SC_Time* Get() { return gInstance; }

	static float gDeltaTime;
	static float gElapsedTime;

	static uint32 gDeltaTimeMS; // In milliseconds
	static uint32 gElapsedTimeMS; // In milliseconds

	static uint32 gFrameCounter;

	static uint32 gFramerate; // not accurate, just as an indication

	void Update(); // ONLY call this ONCE per frame

	SC_Time();
	~SC_Time();
private:

	std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> mLatestTick;

	static constexpr uint32 NumFrameRateSamples = 10;
	uint32 mLocalFramerateSamples[NumFrameRateSamples];

	static SC_Time* gInstance;
};

class SC_Timer
{
public:
	SC_Timer() : mStartTime(std::chrono::high_resolution_clock::now()) {}

	template<class ReturnType>
	ReturnType Stop() const
	{
		auto now = std::chrono::high_resolution_clock::now();
		auto duration = now - mStartTime;
		using ReturnT = std::chrono::duration<ReturnType, std::milli>;
		return std::chrono::duration_cast<ReturnT>(duration).count();
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
};
