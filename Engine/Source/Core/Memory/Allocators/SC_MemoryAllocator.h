#pragma once
#include "Platform/Platform/SC_Platform.h"

class SC_MemoryAllocator
{
public:
	static SC_MemoryAllocator& Get();

	static void* Alloc(size_t aSize, bool aIsArray);
	static void Release(void* aPtr, bool aIsArray);
public:
	SC_MemoryAllocator();
	~SC_MemoryAllocator();

private:
	void* AllocInternal(size_t aSize, bool aIsArray);
	void ReleaseInternal(void* aPtr, bool aIsArray);

	//SC_BlockAllocatorHeap<> mSmallSizeHeap;
	//SC_BlockAllocatorHeap<> mMediumSizeHeap;
	//SC_BlockAllocatorHeap<> mLargeSizeHeap;

};

