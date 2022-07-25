#include "SC_Profiler.h"

#if ENABLE_PROFILER
#include <chrono>

thread_local SC_Profiler::Section* SC_Profiler::gCurrentSection = nullptr;
thread_local SC_Profiler::Scope* SC_Profiler::gCurrentScope = nullptr;

SC_Profiler& SC_Profiler::Get()
{
	static SC_Profiler gInstance;
	return gInstance;
}

SC_Profiler::SC_Profiler()
	: mProfilesWritten(0)
{
	mOutputStream = std::ofstream("profiler_session.json");
	WriteHeader();
}

SC_Profiler::~SC_Profiler()
{
	WriteFooter();
}

void SC_Profiler::BeginSection(const Section& aTag)
{
	if (gCurrentSection)
		delete gCurrentSection;

	gCurrentSection = new Section(aTag);
}

void SC_Profiler::EndSection()
{
	if (gCurrentSection)
		delete gCurrentSection;

	gCurrentSection = nullptr;
}

void SC_Profiler::BeginSession(const std::string& aTag)
{
	SC_MutexLock lock(mMutex);
	mSessionTags[aTag] = SessionTag(aTag);
}

void SC_Profiler::EndSession(const std::string& aTag)
{
	if (mSessionTags.count(aTag) > 0)
	{
		SC_MutexLock lock(mMutex);
		SessionTag& session = mSessionTags[aTag];
		session.mSnapshot.mEndPoint = std::chrono::high_resolution_clock::now();

		WriteProfile(session.mSnapshot);

		mSessionTags.erase(aTag);
	}
}

void SC_Profiler::WriteSnapshot(const SC_ProfilerSnapshot& aSnapshot)
{
	SC_MutexLock lock(mMutex);
	Section section;

	if (gCurrentSection)
		section = *gCurrentSection;

	mProfilerData[section].Add(aSnapshot);
	WriteProfile(aSnapshot);
}

void SC_Profiler::WriteCurrentScope()
{
	if (gCurrentScope)
		gCurrentScope->WriteSnapshot();
}

SC_Profiler::DataContainer& SC_Profiler::GetData()
{
	return mProfilerData;
}

const SC_Profiler::DataContainer& SC_Profiler::GetData() const
{
	return mProfilerData;
}

void SC_Profiler::WriteHeader()
{
	mOutputStream << "{\"otherData\": {},\n\"traceEvents\":[\n";
	mOutputStream.flush();
}

void SC_Profiler::WriteProfile(const SC_ProfilerSnapshot& aSnapshot)
{
	long long start = std::chrono::time_point_cast<std::chrono::microseconds>(aSnapshot.mStartPoint).time_since_epoch().count();
	long long end = std::chrono::time_point_cast<std::chrono::microseconds>(aSnapshot.mEndPoint).time_since_epoch().count();

	if (mProfilesWritten++ > 0)
		mOutputStream << ",";

	mOutputStream << "{\n";
	mOutputStream << "\"cat\":\"function\",\n";
	mOutputStream << "\"dur\":" << (end - start) << ",\n";
	mOutputStream << "\"name\":\"" << aSnapshot.mTag.c_str() << "\",\n";
	mOutputStream << "\"ph\":\"X\",\n";
	mOutputStream << "\"pid\":0,\n";
	mOutputStream << "\"tid\":\"" << aSnapshot.mThreadName.c_str() << "\",\n";
	mOutputStream << "\"ts\":" << start;
	mOutputStream << "\n}\n";

	mOutputStream.flush();

}

void SC_Profiler::WriteFooter()
{
	mOutputStream << "]}";
	mOutputStream.flush();
}


SC_Profiler::Scope::Scope(const char* aTag, bool aWriteSnapshotOnDestruction)
	: mWriteSnapshotOnDestruction(aWriteSnapshotOnDestruction)
{
	mSnapshot.mTag = aTag;
	mSnapshot.mThreadName = SC_Thread::GetCurrentThreadName();
	mSnapshot.mThreadId = (uint32)SC_Thread::GetCurrentThreadId();
	mSnapshot.mStartPoint = std::chrono::high_resolution_clock::now();

	if (!mWriteSnapshotOnDestruction)
		SC_Profiler::Get().gCurrentScope = this;
}

SC_Profiler::Scope::~Scope()
{
	if (mWriteSnapshotOnDestruction)
		WriteSnapshot();
	else
		SC_Profiler::Get().gCurrentScope = nullptr;
}

void SC_Profiler::Scope::WriteSnapshot()
{
	mSnapshot.mEndPoint = std::chrono::high_resolution_clock::now();
	const std::chrono::microseconds deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(mSnapshot.mEndPoint - mSnapshot.mStartPoint);
	mSnapshot.mTimeElapsedMs = static_cast<float>(deltaTime.count() / 1000.f);
	SC_Profiler::Get().WriteSnapshot(mSnapshot);
}

SC_Profiler::SessionTag::SessionTag(const std::string& aTag)
{
	mSnapshot.mTag = aTag;
	mSnapshot.mThreadName = SC_Thread::GetCurrentThreadName();
	mSnapshot.mThreadId = (uint32)SC_Thread::GetCurrentThreadId();
	mSnapshot.mStartPoint = std::chrono::high_resolution_clock::now();
}

#endif //ENABLE_PROFILER

