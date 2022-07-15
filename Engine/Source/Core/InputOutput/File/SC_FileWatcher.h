#pragma once
#include "SC_File.h"
#include "Platform/Async/SC_Thread.h"
#include "Platform/Async/SC_Mutex.h"
#include <filesystem>

struct SR_FileWatchEntry
{
	std::filesystem::path mPath;
	std::filesystem::file_time_type mChangedTimeStamp;
};

struct SC_FileWatcherListener
{
	friend class SC_FileWatcher;
public:
	SC_FileWatcherListener();
	virtual ~SC_FileWatcherListener();

protected:
	enum class ChangeReason
	{
		Changed,
		Added,
		Removed,
	};

	virtual void OnChanged(const SC_FilePath& aPath, const ChangeReason& aReason) = 0;

	void AddWatchEntry(const SC_FilePath& aPath);

private:
	void CheckForChanges();
	std::filesystem::file_time_type mFileChangedTimeStamp;
	SC_Array<SR_FileWatchEntry> mEntries;
};

class SC_FileWatcher : public SC_Thread
{
public:
	static void Create();
	static void Destroy();
	static SC_FileWatcher* Get();

	void AddListener(SC_FileWatcherListener* aListener);
	void RemoveListener(SC_FileWatcherListener* aListener);

private:
	SC_FileWatcher();
	~SC_FileWatcher();

protected:
	void ThreadMain();

private:
	SC_Array<SC_FileWatcherListener*> mListeners;
	SC_Mutex mListenerMutex;

	static constexpr uint32 gUpdateFrequency = 10;
	static SC_FileWatcher* gInstance;
};

