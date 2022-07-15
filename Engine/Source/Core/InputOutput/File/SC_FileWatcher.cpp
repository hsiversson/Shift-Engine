#include "SC_FileWatcher.h"

SC_FileWatcherListener::SC_FileWatcherListener()
{
	if (SC_FileWatcher* watcher = SC_FileWatcher::Get())
		watcher->AddListener(this);
}

SC_FileWatcherListener::~SC_FileWatcherListener()
{
	if (SC_FileWatcher* watcher = SC_FileWatcher::Get())
		watcher->RemoveListener(this);
}

void SC_FileWatcherListener::AddWatchEntry(const SC_FilePath& aPath)
{
	std::filesystem::path path(aPath.GetStr());
	if (!path.empty() && std::filesystem::exists(path))
	{
		std::filesystem::file_time_type latestChangeTimeStamp = std::filesystem::last_write_time(path); 
		mEntries.Add({ path, latestChangeTimeStamp });
	}
}

void SC_FileWatcherListener::CheckForChanges()
{
	for (SR_FileWatchEntry& entry : mEntries)
	{
		std::filesystem::file_time_type latestChangeTimeStamp = std::filesystem::last_write_time(entry.mPath);
		if (entry.mChangedTimeStamp != latestChangeTimeStamp)
		{
			OnChanged(SC_FilePath(entry.mPath.string()), ChangeReason::Changed);
			entry.mChangedTimeStamp = latestChangeTimeStamp;
		}
	}
}


SC_FileWatcher* SC_FileWatcher::gInstance = nullptr;

void SC_FileWatcher::Create()
{
	if (!gInstance)
		gInstance = new SC_FileWatcher;
}

void SC_FileWatcher::Destroy()
{
	delete gInstance;
	gInstance = nullptr;
}

SC_FileWatcher* SC_FileWatcher::Get()
{
	return gInstance;
}

void SC_FileWatcher::AddListener(SC_FileWatcherListener* aListener)
{
	SC_MutexLock lock(mListenerMutex);
	mListeners.AddUnique(aListener);
}

void SC_FileWatcher::RemoveListener(SC_FileWatcherListener* aListener)
{
	SC_MutexLock lock(mListenerMutex);
	mListeners.RemoveCyclic(aListener);
}

SC_FileWatcher::SC_FileWatcher()
{
	SetName("FileWatcher Thread");
	Start();
}

SC_FileWatcher::~SC_FileWatcher()
{
	Stop(true);
}

void SC_FileWatcher::ThreadMain()
{
	while (mIsRunning)
	{
		SC_MutexLock lock(mListenerMutex);
		for (SC_FileWatcherListener* listener : mListeners)
		{
			listener->CheckForChanges();
		}

		Sleep(10);
	}
}
