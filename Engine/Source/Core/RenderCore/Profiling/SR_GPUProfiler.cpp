#include "SR_GPUProfiler.h"

SR_GPUProfiler* SR_GPUProfiler::gInstance = nullptr;

SR_GPUProfiler::SR_GPUProfiler()
{
	SC_ASSERT(gInstance == nullptr, "Only one instance may exist!");
	gInstance = this;
}

SR_GPUProfiler::~SR_GPUProfiler()
{
	gInstance = nullptr;
}

void SR_GPUProfiler::BeginEvent(SR_CommandList* aCmdList, const char* aEventMarker)
{
	aCmdList->BeginEvent(aEventMarker); // Add marker

	// do profiling queries

}

void SR_GPUProfiler::EndEvent(SR_CommandList* aCmdList)
{
	aCmdList->EndEvent();
}

SR_GPUProfiler& SR_GPUProfiler::Get()
{
	return *gInstance;
}

