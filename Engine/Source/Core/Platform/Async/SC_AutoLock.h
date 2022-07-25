#pragma once

template<class Mutex>
class SC_AutoLock : public SC_CopyDisabled
{
public:
	SC_AutoLock() : mInternalMutex(nullptr) {}
	SC_AutoLock(const Mutex& aInMutex) : mInternalMutex(&aInMutex) { mInternalMutex->Lock(); }
	SC_AutoLock(SC_AutoLock&& aInLock) : mInternalMutex(aInLock.mInternalMutex) { aInLock.mInternalMutex = nullptr; }
	~SC_AutoLock() { Unlock(); }

	inline Mutex* GetMutex() const { return mInternalMutex; }

	inline bool IsLocked() const { return mInternalMutex != nullptr; }
	inline bool IsLocked(const Mutex& aInMutex) const { return mInternalMutex == &aInMutex; }

	inline void Lock(Mutex& InMutex)
	{
		assert(mInternalMutex == nullptr);
		mInternalMutex = &InMutex;
		mInternalMutex->Lock();
	}

	inline bool TryLock(Mutex& InMutex)
	{
		assert(mInternalMutex == nullptr);
		if (InMutex.TryLock())
		{
			mInternalMutex = &InMutex;
			return true;
		}
		return false;
	}

	inline void Unlock()
	{
		if (!mInternalMutex)
			return;

		mInternalMutex->Unlock();
		mInternalMutex = nullptr;
	}

private:
	const Mutex* mInternalMutex;
};

template<typename Mutex>
class SC_ReadLock : public SC_CopyDisabled
{
public:
	SC_ReadLock() : mInternalMutex(nullptr) {}
	SC_ReadLock(Mutex& aRwMutex) : mInternalMutex(&aRwMutex) { aRwMutex.BeginRead(); }
	SC_ReadLock(SC_ReadLock<Mutex>&& aRwLock) : mInternalMutex(aRwLock.mMutex) { aRwLock.mInternalMutex = nullptr; }
	~SC_ReadLock() { Unlock(); }

	inline void Lock(Mutex& aRwMutex) 
	{ 
		assert(!mInternalMutex); 
		mInternalMutex = &aRwMutex; 
		aRwMutex.BeginRead(); 
	}

	inline bool TryLock(Mutex& aRwMutex)
	{
		assert(!mInternalMutex);
		if (!aRwMutex.TryBeginRead())
			return false;

		mInternalMutex = &aRwMutex;
		return true;
	}

	inline void Unlock() 
	{ 
		if (mInternalMutex) 
		{ 
			mInternalMutex->EndRead();
			mInternalMutex = nullptr; 
		} 
	}

private:
	Mutex* mInternalMutex;
};

template<typename Mutex>
class SC_WriteLock : public SC_CopyDisabled
{
public:
	SC_WriteLock() : mInternalMutex(nullptr) {}
	SC_WriteLock(Mutex& aRwMutex) : mInternalMutex(&aRwMutex) { aRwMutex.BeginWrite(); }
	SC_WriteLock(SC_ReadLock<Mutex>&& aRwLock) : mInternalMutex(aRwLock.mMutex) { aRwLock.mInternalMutex = nullptr; }
	~SC_WriteLock() { Unlock(); }

	inline void Lock(Mutex& aRwMutex)
	{
		assert(!mInternalMutex);
		mInternalMutex = &aRwMutex;
		aRwMutex.BeginWrite();
	}

	inline bool TryLock(Mutex& aRwMutex)
	{
		assert(!mInternalMutex);
		if (!aRwMutex.TryBeginWrite())
			return false;

		mInternalMutex = &aRwMutex;
		return true;
	}

	inline void Unlock()
	{
		if (mInternalMutex)
		{
			mInternalMutex->EndWrite();
			mInternalMutex = nullptr;
		}
	}

private:
	Mutex* mInternalMutex;
};