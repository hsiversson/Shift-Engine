#pragma once

#if SC_OVERLOAD_NEW_DELETE_OPERATORS
#include "Memory/Allocators/SC_MemoryAllocator.h"
#include "Platform/Debugging/SC_Assert.h"
#include <new>

#pragma warning(push)
#pragma warning(disable: 28251) // Inconsistent annotation for 'new': this instance has no annotations

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