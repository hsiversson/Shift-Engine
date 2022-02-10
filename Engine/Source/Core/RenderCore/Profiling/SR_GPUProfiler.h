#pragma once

#if ENABLE_GPU_PROFILER
class SR_CommandList;

class SR_GPUProfiler
{
public:
	SR_GPUProfiler();
	~SR_GPUProfiler();

	void BeginEvent(SR_CommandList* aCmdList, const char* aEventMarker);
	void EndEvent(SR_CommandList* aCmdList);

	// GetEventTimers

	static SR_GPUProfiler& Get();

private:

	static SR_GPUProfiler* gInstance;
};

class SR_GPUProfilerEventScope
{
public:
	SR_GPUProfilerEventScope(SR_CommandList* aCmdList, const char* aTag)
		: mCmdList(aCmdList)
		, mTag(aTag)
	{
		SR_GPUProfiler::Get().BeginEvent(mCmdList, mTag);
	}

	~SR_GPUProfilerEventScope()
	{
		SR_GPUProfiler::Get().EndEvent(mCmdList);
	}
private:
	SR_CommandList* mCmdList;
	const char* mTag;
};

#define SR_PROFILE_SCOPED(aCommandList, aTag)		SR_GPUProfilerEventScope __profilerScope_##aTag(aCommandList, aTag)
#define SR_PROFILE_FUNCTION(aCommandList)			SR_PROFILE_SCOPED(aCommandList, __FUNCTION__)

#else //ENABLE_GPU_PROFILER

#define SR_PROFILE_SCOPED(aCommandList, aTag)
#define SR_PROFILE_FUNCTION(aCommandList)

#endif //ENABLE_GPU_PROFILER