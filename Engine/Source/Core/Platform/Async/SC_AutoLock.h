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