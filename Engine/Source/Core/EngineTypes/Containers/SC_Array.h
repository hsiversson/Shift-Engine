#pragma once
#include "Platform/Misc/SC_Relocation.h"

#include <initializer_list>
#include <functional>

#define SC_ARRAY_BOUNDS_CHECK(aIdx, aSize) SC_ASSERT(aIdx < aSize, "Out of bounds! (anIndex: {0} Size: {1})", aIdx, aSize);

template<class T>
class SC_Array
{
	template<class Type>
	using EqualType = decltype(std::declval<const Type>() == std::declval<const Type>());

	template<class Type>
	static constexpr bool gHasEqualType = SC_DetectionIdiom<EqualType, Type>::mValue;

public:
	SC_Array();
	SC_Array(uint32 aReservedCount);
	SC_Array(const SC_Array& aRhs);
	SC_Array(SC_Array&& aRhs) noexcept;
	SC_Array(std::initializer_list<T> aInitList);
	virtual ~SC_Array();

	uint32 Count() const;
	int32 ICount() const;
	uint32 ElementStride() const;
	uint32 GetByteSize() const;
	uint32 Capacity() const;
	bool IsEmpty() const;

	void Reserve(uint32 aNumToReserve);
	void Respace(uint32 aNumToRespace);
	void AllocateAdditional(uint32 aNumToAlloc);

	T& First();
	const T& First() const;
	T& Last();
	const T& Last() const;

	T* begin();
	const T* begin() const;
	T* end();
	const T* end() const;

	void ForEach(std::function<void(T&)> aLambda);
	int32 Find(const T& aItem, uint32 aSearchStartIndex = 0) const;

	void Swap(SC_Array<T>& aOther);

	T& Add();
	void AddUninitialized(uint32 aNumToAdd = 1);
	T& Add(const T& aItem);
	T& Add(T&& aItem);
	bool AddUnique(const T& aItem);
	void Add(const SC_Array& aOtherArray);
	void Add(const T* aItemBuffer, uint32 aNumItemsToAdd);
	T& AddAt(uint32 aIndex, const T& aItem);

	void Remove(const T& aItem);
	void RemoveCyclic(const T& aItem);
	void RemoveAt(uint32 aIndex);
	void RemoveLast();
	void RemoveCyclicAt(uint32 aIndex);
	void RemoveAll();

	void Reset();

	void operator+=(const SC_Array<T>& aRhs);
	void operator+=(SC_Array<T>&& aRhs);
	void operator=(const SC_Array<T>& aRhs);
	void operator=(SC_Array<T>&& aRhs) noexcept;

	T& operator[](uint32 aIndex);
	const T& operator[](uint32 aIndex) const;

	T* GetBuffer();
	const T* GetBuffer() const;
	T* operator*();
	const T* operator*() const;

	static constexpr int32 gFindResultNone = -1;
protected:

	T* Allocate(uint32 aNumItems);
	void Deallocate(T* aBuffer);

	void Destruct(T* aStartPtr, uint32 aNumItems);

	T* mInternalItemBuffer;
	uint32 mCurrentItemCount;
	uint32 mItemCapacity;
};

template <class T>
struct SC_AllowMemcpyRelocation<SC_Array<T>>
{
	static const bool mValue = true;
};

template <class T>
SC_Array<T>::SC_Array()
	: mInternalItemBuffer(nullptr)
	, mCurrentItemCount(0)
	, mItemCapacity(0)
	//, mIsHybrid(false)
{
}

template <class T>
SC_Array<T>::SC_Array(uint32 aReservedCount)
	: mInternalItemBuffer(nullptr)
	, mCurrentItemCount(0)
	, mItemCapacity(0)
{
	Respace(aReservedCount);
}

template <class T>
SC_Array<T>::SC_Array(const SC_Array& aRhs)
	: mInternalItemBuffer(nullptr)
	, mCurrentItemCount(aRhs.mCurrentItemCount)
	, mItemCapacity(0)
	//, mIsHybrid(false)
{
	Reserve(mCurrentItemCount);
	if (mCurrentItemCount)
	{
		SC_CopyConstruct(mInternalItemBuffer, aRhs.mInternalItemBuffer, mCurrentItemCount);
	}
}

template <class T>
SC_Array<T>::SC_Array(SC_Array&& aRhs) noexcept
	: mInternalItemBuffer(aRhs.mInternalItemBuffer)
	, mCurrentItemCount(aRhs.mCurrentItemCount)
	, mItemCapacity(aRhs.mItemCapacity)
	//, mIsHybrid(false)
{
	aRhs.mInternalItemBuffer = nullptr;
	aRhs.mCurrentItemCount = 0;
	aRhs.mItemCapacity = 0;
}

template<class T>
SC_Array<T>::SC_Array(std::initializer_list<T> aInitList)
	: mInternalItemBuffer(nullptr)
	, mCurrentItemCount((uint32)aInitList.size())
	, mItemCapacity(0)
	//, mIsHybrid(false)
{
	Reserve(mCurrentItemCount);
	if (mCurrentItemCount)
	{
		SC_CopyConstruct(mInternalItemBuffer, aInitList.begin(), mCurrentItemCount);
	}
}

template <class T>
SC_Array<T>::~SC_Array()
{
	Reset();
}

template <class T>
inline uint32 SC_Array<T>::Count() const
{
	return mCurrentItemCount;
}

template<class T>
inline int32 SC_Array<T>::ICount() const
{
	return int32(mCurrentItemCount);
}

template <class T>
inline uint32 SC_Array<T>::ElementStride() const
{
	return sizeof(T);
}

template<class T>
inline uint32 SC_Array<T>::GetByteSize() const
{
	return ElementStride() * mCurrentItemCount;
}

template <class T>
inline uint32 SC_Array<T>::Capacity() const
{
	return mItemCapacity;
}

template<class T>
inline bool SC_Array<T>::IsEmpty() const
{
	return (mCurrentItemCount == 0);
}

template <class T>
inline void SC_Array<T>::Reserve(uint32 aNumToReserve)
{
	if (aNumToReserve <= mItemCapacity)
		return;

	T* oldData = mInternalItemBuffer;
	mInternalItemBuffer = Allocate(aNumToReserve);
	mItemCapacity = aNumToReserve;

	if (oldData)
	{
		SC_RelocateN(mInternalItemBuffer, oldData, mCurrentItemCount);
		//if (!IsHybridBuffer(oldData))
		//{
			Deallocate(oldData);
		//}
	}
}

template <class T>
inline void SC_Array<T>::Respace(uint32 aNumToRespace)
{
	if (aNumToRespace == mCurrentItemCount)
		return;

	Reserve(aNumToRespace);

	if (aNumToRespace > mCurrentItemCount)
		SC_Construct(mInternalItemBuffer + mCurrentItemCount, aNumToRespace - mCurrentItemCount);
	else
		Destruct(mInternalItemBuffer + aNumToRespace, mCurrentItemCount - aNumToRespace);

	mCurrentItemCount = aNumToRespace;
}

template<class T>
inline void SC_Array<T>::AllocateAdditional(uint32 aNumToAlloc)
{
	const uint32 requestedCount = mCurrentItemCount + aNumToAlloc;

	if (requestedCount <= mItemCapacity)
		return;

	uint32 increaseCount = mCurrentItemCount + ((mCurrentItemCount + 1) / 2);
	uint32 newAllocatedCount = SC_Max(increaseCount, requestedCount);

	Reserve(newAllocatedCount);
}

template <class T>
inline T& SC_Array<T>::First()
{
	SC_ARRAY_BOUNDS_CHECK(0, mCurrentItemCount);
	return mInternalItemBuffer[0];
}

template <class T>
inline const T& SC_Array<T>::First() const
{
	SC_ARRAY_BOUNDS_CHECK(0, mCurrentItemCount);
	return mInternalItemBuffer[0];
}

template <class T>
inline const T& SC_Array<T>::Last() const
{
	SC_ARRAY_BOUNDS_CHECK(mCurrentItemCount - 1u, mCurrentItemCount);
	return mInternalItemBuffer[mCurrentItemCount - 1];
}

template <class T>
inline T& SC_Array<T>::Last()
{
	SC_ARRAY_BOUNDS_CHECK(mCurrentItemCount - 1u, mCurrentItemCount);
	return mInternalItemBuffer[mCurrentItemCount - 1];
}

template <class T>
inline T* SC_Array<T>::begin()
{
	return mInternalItemBuffer;
}

template <class T>
inline const T* SC_Array<T>::begin() const
{
	return mInternalItemBuffer;
}

template <class T>
inline T* SC_Array<T>::end()
{
	return mInternalItemBuffer + mCurrentItemCount;
}

template <class T>
inline const T* SC_Array<T>::end() const
{
	return mInternalItemBuffer + mCurrentItemCount;
}

template<class T>
inline void SC_Array<T>::ForEach(std::function<void(T&)> aLambda)
{
	for (uint32 i = 0; i < mCurrentItemCount; ++i)
	{
		aLambda(mInternalItemBuffer[i]);
	}
}

template<class T>
inline int32 SC_Array<T>::Find(const T& aItem, uint32 aSearchStartIndex) const
{
	if (aSearchStartIndex >= mCurrentItemCount)
		return gFindResultNone;

	if constexpr (gHasEqualType<T>)
	{
		for (uint32 i = aSearchStartIndex; i < mCurrentItemCount; ++i)
		{
			if (mInternalItemBuffer[i] == aItem)
			{
				return i;
			}
		}
		return gFindResultNone;
	}
	else
	{
		return gFindResultNone;
	}
}

template<class T>
inline void SC_Array<T>::Swap(SC_Array<T>& aOther)
{
	const size_t sizeofThis = sizeof(*this);
	char tmp[sizeofThis] = {};
	SC_Memcpy(tmp, &aOther, sizeofThis);
	SC_Memcpy(&aOther, this, sizeofThis);
	SC_Memcpy(this, tmp, sizeofThis);
}

template<class T>
inline T& SC_Array<T>::Add()
{
	AllocateAdditional(1);

	const uint32 currentCount = mCurrentItemCount;
	new (const_cast<typename SC_RemoveConst<T>::Type*>(mInternalItemBuffer + currentCount)) T();
	mCurrentItemCount = currentCount + 1;
	return mInternalItemBuffer[currentCount];
}

template<class T>
inline void SC_Array<T>::AddUninitialized(uint32 aNumToAdd)
{
	AllocateAdditional(aNumToAdd);
	mCurrentItemCount += aNumToAdd;
}

template <class T>
inline T& SC_Array<T>::Add(const T& aItem)
{
	AllocateAdditional(1);

	const uint32 currentCount = mCurrentItemCount;
	new (const_cast<typename SC_RemoveConst<T>::Type*>(mInternalItemBuffer + currentCount)) T(aItem);
	mCurrentItemCount = currentCount + 1;
	return mInternalItemBuffer[currentCount];
}

template <class T>
inline T& SC_Array<T>::Add(T&& aItem)
{
	AllocateAdditional(1);

	const uint32 currentCount = mCurrentItemCount;
	new (const_cast<typename SC_RemoveConst<T>::Type*>(mInternalItemBuffer + currentCount)) T(SC_Move(aItem));
	mCurrentItemCount = currentCount + 1;
	return mInternalItemBuffer[currentCount];
}

template<class T>
inline bool SC_Array<T>::AddUnique(const T& aItem)
{
	if (Find(aItem) == gFindResultNone)
	{
		Add(aItem);
		return true;
	}

	return false;
}

template <class T>
inline void SC_Array<T>::Add(const SC_Array& aOtherArray)
{
	if (aOtherArray.mCurrentItemCount == 0)
		return;

	AllocateAdditional(aOtherArray.mCurrentItemCount);
	SC_CopyConstruct(mInternalItemBuffer + mCurrentItemCount, aOtherArray.mInternalItemBuffer, aOtherArray.mCurrentItemCount);
	mCurrentItemCount += aOtherArray.mCurrentItemCount;
}

template <class T>
inline void SC_Array<T>::Add(const T* aItemBuffer, uint32 aNumItemsToAdd)
{
	if (aNumItemsToAdd == 0)
		return;

	AllocateAdditional(aNumItemsToAdd);
	SC_CopyConstruct(mInternalItemBuffer + mCurrentItemCount, aItemBuffer, aNumItemsToAdd);
	mCurrentItemCount += aNumItemsToAdd;
}

template<class T>
inline T& SC_Array<T>::AddAt(uint32 aIndex, const T& aItem)
{
	SC_ARRAY_BOUNDS_CHECK(aIndex, mCurrentItemCount);
	mInternalItemBuffer[aIndex] = aItem;
	return mInternalItemBuffer[aIndex];
}

template <class T>
inline void SC_Array<T>::Remove(const T& aItem)
{
	int32 index = Find(aItem);
	if (index == gFindResultNone)
		return;

	RemoveAt(index);
}

template<class T>
inline void SC_Array<T>::RemoveCyclic(const T& aItem)
{
	int32 index = Find(aItem);
	if (index == gFindResultNone)
		return;

	RemoveCyclicAt(index);
}

template <class T>
inline void SC_Array<T>::RemoveAt(uint32 aIndex)
{
	SC_ARRAY_BOUNDS_CHECK(aIndex, mCurrentItemCount);

	Destruct((mInternalItemBuffer + aIndex), 1);
	SC_RelocateN((mInternalItemBuffer + aIndex), (mInternalItemBuffer + aIndex + 1), (mCurrentItemCount - aIndex - 1));

	--mCurrentItemCount;
}

template <class T>
inline void SC_Array<T>::RemoveLast()
{
	RemoveCyclicAt(mCurrentItemCount - 1);
}

template<class T>
inline void SC_Array<T>::RemoveCyclicAt(uint32 aIndex)
{
	SC_ARRAY_BOUNDS_CHECK(aIndex, mCurrentItemCount);

	Destruct((mInternalItemBuffer + aIndex), 1);
	SC_Relocate((mInternalItemBuffer + aIndex), (mInternalItemBuffer + this->mCurrentItemCount - 1));
	--mCurrentItemCount;
}

template <class T>
inline void SC_Array<T>::RemoveAll()
{
	Destruct(mInternalItemBuffer, mCurrentItemCount);
	mCurrentItemCount = 0;
}

template<class T>
inline void SC_Array<T>::Reset()
{
	RemoveAll();
	if (mInternalItemBuffer /*&& !IsHybridBuffer(mInternalItemBuffer)*/)
	{
		Deallocate(mInternalItemBuffer);
		mInternalItemBuffer = nullptr;

		mCurrentItemCount = 0;
		mItemCapacity = 0;
	}
	//else if (mIsHybrid)
	//{
	//	mInternalItemBuffer = GetHybridBuffer();
	//	mItemCapacity = GetHybridBufferSize();
	//}
}


template <class T>
inline void SC_Array<T>::operator+=(const SC_Array<T>& aRhs)
{
	Add(aRhs);
}

template <class T>
inline void SC_Array<T>::operator+=(SC_Array<T>&& aRhs)
{
	if (aRhs.mCurrentItemCount == 0)
		return;

	AllocateAdditional(aRhs.mCurrentItemCount);
	SC_MoveConstruct(mInternalItemBuffer + mCurrentItemCount, aRhs.mInternalItemBuffer, aRhs.mCurrentItemCount);
	mCurrentItemCount += aRhs.mCurrentItemCount;

	aRhs.mCurrentItemCount = 0;
	aRhs.mItemCapacity = 0;
	Deallocate(aRhs.mInternalItemBuffer);
	aRhs.mInternalItemBuffer = nullptr;
}

template <class T>
inline void SC_Array<T>::operator=(const SC_Array<T>& aRhs)
{
	if (this == &aRhs)
		return;

	mCurrentItemCount = aRhs.mCurrentItemCount;
	Reserve(mCurrentItemCount);
	if (mCurrentItemCount)
	{
		SC_CopyConstruct(mInternalItemBuffer, aRhs.mInternalItemBuffer, mCurrentItemCount);
	}
}

template <class T>
inline void SC_Array<T>::operator=(SC_Array<T>&& aRhs) noexcept
{
	if (this == &aRhs)
		return;

	RemoveAll();

	mInternalItemBuffer = aRhs.mInternalItemBuffer;
	mCurrentItemCount = aRhs.mCurrentItemCount;
	mItemCapacity = aRhs.mItemCapacity;

	aRhs.mInternalItemBuffer = nullptr;
	aRhs.mCurrentItemCount = 0;
	aRhs.mItemCapacity = 0;
}

template <class T>
inline T& SC_Array<T>::operator[](uint32 aIndex)
{
	SC_ARRAY_BOUNDS_CHECK(aIndex, mCurrentItemCount);
	return mInternalItemBuffer[aIndex];
}

template <class T>
inline const T& SC_Array<T>::operator[](uint32 aIndex) const
{
	SC_ARRAY_BOUNDS_CHECK(aIndex, mCurrentItemCount);
	return mInternalItemBuffer[aIndex];
}

template <class T>
T* SC_Array<T>::GetBuffer()
{
	return mInternalItemBuffer;
}

template <class T>
const T* SC_Array<T>::GetBuffer() const
{
	return mInternalItemBuffer;
}

template <class T>
T* SC_Array<T>::operator*()
{
	return mInternalItemBuffer;
}

template <class T>
const T* SC_Array<T>::operator*() const
{
	return mInternalItemBuffer;
}

//template<class T>
//inline bool SC_Array<T>::IsHybridBuffer(T* aBuffer)
//{
//	if (!this->mIsHybrid)
//		return false;
//
//	return aBuffer == GetHybridBuffer();
//}
//
//template<class T>
//inline uint32 SC_Array<T>::GetHybridBufferSize()
//{
//	SC_ASSERT(mIsHybrid && "This is not a HybridArray");
//	return 0;
//}
//
//template<class T>
//inline T* SC_Array<T>::GetHybridBuffer()
//{
//	SC_ASSERT(mIsHybrid && "This is not a HybridArray");
//	return nullptr;
//}

template<class T>
inline T* SC_Array<T>::Allocate(uint32 aNumItems)
{
	if (aNumItems == 0)
		return nullptr;

	return (T*) new char[sizeof(T) * aNumItems];
}

template<class T>
inline void SC_Array<T>::Deallocate(T* aBuffer)
{
	delete[](char*)aBuffer;
}

template<class T>
inline void SC_Array<T>::Destruct(T* aStartPtr, uint32 aNumItems)
{
	(void)aStartPtr;
	for (int i = aNumItems - 1; i >= 0; --i)
	{
		(aStartPtr + i)->~T();
	}
}