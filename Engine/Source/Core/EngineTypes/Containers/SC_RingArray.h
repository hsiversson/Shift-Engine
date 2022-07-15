#pragma once

template<class T>
class SC_RingArray
{
public:
	SC_RingArray();
	SC_RingArray(const SC_RingArray& aOther) = delete;
	SC_RingArray(SC_RingArray&& aOther);
	~SC_RingArray();

	void operator=(const SC_RingArray& aOther) = delete;

	void Reserve(uint32 aCount);

	bool IsEmpty() const;
	uint32 Count() const;
	int32 ICount() const;
	uint32 Capacity() const;

	T& Add();
	T& Add(const T& aItem);

	T& Peek() const;
	T& operator[](uint32 aIndex);
	const T& operator[](uint32 aIndex) const;

	void Remove();
	void RemoveAll();
	void Clear();

private:
	T* mInternalItemBuffer;
	uint32 mItemCapacity;
	uint32 mCurrentItemCount;
	uint32 mBeginIndex; // First Index of contiguous array of items 
	uint32 mEndIndex; // Last Index of contiguous array of items 
};

template<class T>
inline SC_RingArray<T>::SC_RingArray()
	: mInternalItemBuffer(nullptr)
	, mItemCapacity(1)
	, mCurrentItemCount(0)
	, mBeginIndex(0)
	, mEndIndex(0)
{
}

template<class T>
inline SC_RingArray<T>::SC_RingArray(SC_RingArray&& aOther)
	: mInternalItemBuffer(aOther.mInternalItemBuffer)
	, mItemCapacity(aOther.mItemCapacity)
	, mCurrentItemCount(aOther.mCurrentItemCount)
	, mBeginIndex(aOther.mReadIndex)
	, mEndIndex(aOther.mWriteIndex)
{
	aOther.mInternalItemBuffer = nullptr;
	aOther.mItemCapacity = 1;
	aOther.mCurrentItemCount = 0;
	aOther.mReadIndex = 0;
	aOther.mWriteIndex = 0;
}

template<class T>
inline SC_RingArray<T>::~SC_RingArray()
{
	RemoveAll();
	delete[] (char*)mInternalItemBuffer;
}

template<class T>
inline void SC_RingArray<T>::Reserve(uint32 aCount)
{
	if (aCount <= mItemCapacity)
		return;

	T* newData = (T*) new char[aCount * sizeof(T)];

	uint32 read = mBeginIndex;
	uint32 write = mEndIndex;
	uint32 count = 0;
	if (read < write)
	{
		count = write - read;
		SC_RelocateN(newData, mInternalItemBuffer + read, write - read);
	}
	else if (read > write)
	{
		uint32 endCount = mItemCapacity - read;
		count = endCount + write;
		SC_ASSERT(count == mCurrentItemCount);
		SC_RelocateN(newData, mInternalItemBuffer + read, endCount);
		SC_RelocateN(newData + endCount, mInternalItemBuffer, write);
	}

	delete[] (char*)mInternalItemBuffer;

	mInternalItemBuffer = newData;
	mItemCapacity = aCount;
	mBeginIndex = 0;
	mEndIndex = count;
}

template<class T>
inline bool SC_RingArray<T>::IsEmpty() const
{
	return mCurrentItemCount == 0;
}

template<class T>
inline uint32 SC_RingArray<T>::Count() const
{
	return mCurrentItemCount;
}

template<class T>
inline int32 SC_RingArray<T>::ICount() const
{
	return int32(mCurrentItemCount);
}

template<class T>
inline uint32 SC_RingArray<T>::Capacity() const
{
	return mItemCapacity;
}

template<class T>
inline T& SC_RingArray<T>::Add()
{
	uint32 write = mEndIndex;
	uint32 next = write + 1;
	uint32 capacity = mItemCapacity;
	if (next == capacity)
		next = 0;

	if (next == mBeginIndex)
	{
		uint32 newCapacity = capacity == 1 ? 32 : capacity * 2;
		Reserve(newCapacity);

		write = mEndIndex;
		next = write + 1;
		if (next == newCapacity)
			next = 0;
	}

	new (static_cast<void*>(mInternalItemBuffer + write)) T();
	mEndIndex = next;

	mCurrentItemCount++;
	return *(mInternalItemBuffer + write);
}

template<class T>
inline T& SC_RingArray<T>::Add(const T& aItem)
{
	uint32 write = mEndIndex;
	uint32 next = write + 1;
	uint32 capacity = mItemCapacity;
	if (next == capacity)
		next = 0;

	if (next == mBeginIndex)
	{
		uint32 newCapacity = capacity == 1 ? 32 : capacity * 2;
		Reserve(newCapacity);

		write = mEndIndex;
		next = write + 1;
		if (next == newCapacity)
			next = 0;
	}

	new (static_cast<void*>(mInternalItemBuffer + write)) T(aItem);
	mEndIndex = next;

	mCurrentItemCount++;
	return *(mInternalItemBuffer + write);
}

template<class T>
inline T& SC_RingArray<T>::Peek() const
{
	SC_ASSERT(mBeginIndex != mEndIndex); 
	return mInternalItemBuffer[mBeginIndex];
}

template<class T>
inline T& SC_RingArray<T>::operator[](uint32 aIndex)
{
	return mInternalItemBuffer[(mBeginIndex + aIndex) % mItemCapacity];
}

template<class T>
inline const T& SC_RingArray<T>::operator[](uint32 aIndex) const
{
	return mInternalItemBuffer[(mBeginIndex + aIndex) % mItemCapacity];
}

template<class T>
inline void SC_RingArray<T>::Remove()
{
	SC_ASSERT(mBeginIndex != mEndIndex);

	(mInternalItemBuffer + mBeginIndex)->~T();

	uint32 read = mBeginIndex + 1;
	mBeginIndex = (read == mItemCapacity) ? 0 : read;
	mCurrentItemCount--;
}

template<class T>
inline void SC_RingArray<T>::RemoveAll()
{
	while (mBeginIndex != mEndIndex)
		Remove();
}

template<class T>
inline void SC_RingArray<T>::Clear()
{
	mBeginIndex = 0;
	mEndIndex = 0;
	mCurrentItemCount = 0;
}

template<class T>
struct SC_AllowMemcpyRelocation<SC_RingArray<T>>
{
	static const bool mValue = true;
};