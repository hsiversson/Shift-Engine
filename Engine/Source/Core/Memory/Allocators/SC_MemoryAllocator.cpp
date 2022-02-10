#include "SC_MemoryAllocator.h"
#include "GlobalData/SC_GlobalData.h"
#include "Misc/SC_NewDeleteOperators.h"

SC_MemoryAllocator& SC_MemoryAllocator::Get()
{
	return SC_GetGlobalData().mMemoryAllocator;
}

void* SC_MemoryAllocator::Alloc(size_t aSize, bool aIsArray)
{
	SC_InitGlobalData();

	SC_MemoryAllocator& inst = Get();
	return inst.AllocInternal(aSize, aIsArray);
}

void SC_MemoryAllocator::Release(void* aPtr, bool aIsArray)
{
	SC_MemoryAllocator& inst = Get();
	inst.ReleaseInternal(aPtr, aIsArray);
}

SC_MemoryAllocator::~SC_MemoryAllocator()
{

}

SC_MemoryAllocator::SC_MemoryAllocator()
{

}

void* SC_MemoryAllocator::AllocInternal(size_t aSize, bool /*aIsArray*/)
{
	return malloc(aSize);
}

void SC_MemoryAllocator::ReleaseInternal(void* aPtr, bool /*aIsArray*/)
{
	free(aPtr);
}
