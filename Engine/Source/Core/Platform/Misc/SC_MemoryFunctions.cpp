#include "SC_MemoryFunctions.h"
#include <cstring>

void SC_Memcpy(void* aDst, const void* aSrc, uint64 aSize)
{
	memcpy(aDst, aSrc, aSize);
}

void SC_Memmove(void* aDst, void* aSrc, uint64 aSize)
{
	memmove(aDst, aSrc, aSize);
}

void SC_Memset(void* aDst, int aValue, uint64 aSize)
{
	memset(aDst, aValue, aSize);
}

void SC_ZeroMemory(void* aDst, uint64 aSize)
{
	memset(aDst, 0, aSize);
}
