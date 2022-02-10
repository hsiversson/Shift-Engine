#include "SC_Time.h"
#include "Platform/Misc/SC_MemoryFunctions.h"

#include <time.h>

float SC_Time::gDeltaTime;
float SC_Time::gElapsedTime;
uint32 SC_Time::gDeltaTimeMS;
uint32 SC_Time::gElapsedTimeMS;
uint32 SC_Time::gFrameCounter;
uint32 SC_Time::gFramerate;

void SC_Time::Update()
{
	using secf = std::chrono::duration<float>;
	using millisecf = std::chrono::duration<float, std::milli>;
	using millisecu = std::chrono::duration<uint32, std::milli>;

	auto now = std::chrono::high_resolution_clock::now();
	auto duration = now - mLatestTick;
	gDeltaTime = std::chrono::duration_cast<secf>(duration).count();
	gElapsedTime += gDeltaTime;
	gDeltaTimeMS = std::chrono::duration_cast<millisecu>(duration).count();
	gElapsedTimeMS += gDeltaTimeMS;
	++gFrameCounter;

	mLocalFramerateSamples[gFrameCounter % NumFrameRateSamples] = uint32((1.f / std::chrono::duration_cast<millisecf>(duration).count()) * 1000.f);

	float framerate = 0.f;
	for (uint32 i = 0; i < NumFrameRateSamples; ++i)
		framerate += mLocalFramerateSamples[i];

	gFramerate = uint32(framerate / NumFrameRateSamples);

	mLatestTick = now;
}

SC_Time::SC_Time()
{
	gInstance = this;
	gDeltaTime = 0.0f;
	gElapsedTime = 0.0f;
	gDeltaTimeMS = 0;
	gElapsedTimeMS = 0;
	gFrameCounter = 0;
	gFramerate = 0;

	mStartTime = std::chrono::high_resolution_clock::now();
	mLatestTick = mStartTime;

	SC_Fill(mLocalFramerateSamples, 10, 0);
}

SC_Time::~SC_Time()
{
	gInstance = nullptr;
}

SC_Time* SC_Time::gInstance;
