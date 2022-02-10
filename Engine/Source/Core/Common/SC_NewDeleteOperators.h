#pragma once

#include "SC_MemoryAllocator.h"

#if SC_OVERLOAD_NEW_DELETE_OPERATORS

#pragma warning(push)
#pragma warning(disable: 28251) // Inconsistent annotation for 'new': this instance has no annotations

#ifdef new
#undef new
#endif

void* operator new(size_t aSize)
{
	return SC_MemoryAllocator::Alloc(aSize, false);
}

void* operator new[](size_t aSize)
{
	return SC_MemoryAllocator::Alloc(aSize, true);
}

void* operator new(size_t aSize, const std::nothrow_t&) noexcept
{
	return SC_MemoryAllocator::Alloc(aSize, false);
}

void* operator new[](size_t aSize, const std::nothrow_t&) noexcept
{
	return SC_MemoryAllocator::Alloc(aSize, true);
}

void* operator new(size_t aSize, std::align_val_t aAlignment)
{
	(void)aAlignment;
	assert((uint64)aAlignment <= 16); 
	return SC_MemoryAllocator::Alloc(aSize, false);
}

void operator delete(void* aPtr) noexcept
{
	SC_MemoryAllocator::Release(aPtr, false);
}

void operator delete[](void* aPtr) noexcept
{
	SC_MemoryAllocator::Release(aPtr, true);
}

void operator delete(void* aPtr, size_t /*aSize*/)
{
	SC_MemoryAllocator::Release(aPtr, false);
}

void operator delete[](void* aPtr, size_t /*aSize*/)
{
	SC_MemoryAllocator::Release(aPtr, true);
}

#pragma warning(pop)
#endif