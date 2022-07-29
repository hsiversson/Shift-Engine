#pragma once

#if ENABLE_PROFILER

struct SC_ProfilerSnapshot
{
	std::string mTag;
	std::string mThreadName;
	uint32 mThreadId;
	float mTimeElapsedMs;
	std::chrono::high_resolution_clock::time_point mStartPoint;
	std::chrono::high_resolution_clock::time_point mEndPoint;

	bool operator<(const SC_ProfilerSnapshot& aOther) const { return (mStartPoint < aOther.mStartPoint); }
};

class SC_Profiler
{
public:
	using Section = std::string;
	using DataContainer = SC_UnorderedMap<Section, SC_Array<SC_ProfilerSnapshot>>;
	struct Scope
	{
		Scope(const char* aTag, bool aWriteSnapshotOnDestruction);
		~Scope();

		void WriteSnapshot();

		SC_ProfilerSnapshot mSnapshot;
		const bool mWriteSnapshotOnDestruction;
	};

	struct SessionTag
	{
		SessionTag() {}
		SessionTag(const std::string& aTag);
		SC_ProfilerSnapshot mSnapshot;
	};
	using SessionDataContainer = SC_UnorderedMap<std::string, SessionTag>;

public:
	static SC_Profiler& Get();

	SC_Profiler();
	~SC_Profiler();

	void BeginSection(const Section& aTag);
	void EndSection();

	void BeginSession(const std::string& aTag);
	void EndSession(const std::string& aTag);

	void WriteSnapshot(const SC_ProfilerSnapshot& aSnapshot);
	void WriteCurrentScope();
	
	DataContainer& GetData();
	const DataContainer& GetData() const;

private:
	SessionDataContainer mSessionTags;
	DataContainer mProfilerData;
	SC_Mutex mMutex;
	std::ofstream mOutputStream;
	uint32 mProfilesWritten;

	void WriteHeader();
	void WriteProfile(const SC_ProfilerSnapshot& aSnapshot);
	void WriteFooter();

	static thread_local Section* gCurrentSection;
	static thread_local Scope* gCurrentScope;
};

#define SC_INTERNAL_SCOPED_PROFILER_FUNCTION_CPU(aTag, aID, aWriteSnapshotOnDestruction) SC_Profiler::Scope _profilerScopedVar##aID(aTag, aWriteSnapshotOnDestruction)

#define SC_PROFILER_FUNCTION() SC_INTERNAL_SCOPED_PROFILER_FUNCTION_CPU(__FUNCTION__, __LINE__, true)

#define SC_PROFILER_EVENT_START(aTag) SC_INTERNAL_SCOPED_PROFILER_FUNCTION_CPU(aTag, __LINE__, false)
#define SC_PROFILER_EVENT_END() SC_Profiler::Get().WriteCurrentScope()

#define SC_PROFILER_BEGIN_SESSION(aTag) SC_Profiler::Get().BeginSession(aTag)
#define SC_PROFILER_END_SESSION(aTag) SC_Profiler::Get().EndSession(aTag);

#define SC_PROFILER_BEGIN_SECTION(aTag) SC_Profiler::Get().BeginSection(aTag)
#define SC_PROFILER_END_SECTION() SC_Profiler::Get().EndSection()

#else //ENABLE_PROFILER

#define SC_PROFILER_FUNCTION() 

#define SC_PROFILER_EVENT_START(aTag) SC_UNUSED(aTag)
#define SC_PROFILER_EVENT_END() 

#define SC_PROFILER_BEGIN_SESSION(aTag) SC_UNUSED(aTag)
#define SC_PROFILER_END_SESSION(aTag) SC_UNUSED(aTag)

#define SC_PROFILER_BEGIN_SECTION(aTag) SC_UNUSED(aTag)
#define SC_PROFILER_END_SECTION() 

#endif //ENABLE_PROFILER