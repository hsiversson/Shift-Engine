
#include "SC_Atomics_Win64.h"

#if IS_WINDOWS_PLATFORM
#include <intrin.h>

#pragma intrinsic (_InterlockedExchangePointer)
#pragma intrinsic (_InterlockedCompareExchangePointer)

// signed/unsigned int8
#pragma intrinsic (_InterlockedCompareExchange8)
#pragma intrinsic (_InterlockedExchange8)
#pragma intrinsic (_InterlockedExchangeAdd8)
#pragma intrinsic (_InterlockedAnd8)
#pragma intrinsic (_InterlockedOr8)

// signed/unsigned int16
#pragma intrinsic (_InterlockedCompareExchange16)
#pragma intrinsic (_InterlockedExchange16)
#pragma intrinsic (_InterlockedIncrement16)
#pragma intrinsic (_InterlockedDecrement16)
#pragma intrinsic (_InterlockedExchangeAdd16)
#pragma intrinsic (_InterlockedAnd16)
#pragma intrinsic (_InterlockedOr16)

// signed/unsigned int32
#pragma intrinsic (_InterlockedCompareExchange)
#pragma intrinsic (_InterlockedExchange)
#pragma intrinsic (_InterlockedIncrement)
#pragma intrinsic (_InterlockedDecrement)
#pragma intrinsic (_InterlockedExchangeAdd)
#pragma intrinsic (_InterlockedAnd)
#pragma intrinsic (_InterlockedOr)

// signed/unsigned int64
#pragma intrinsic (_InterlockedCompareExchange64)
#pragma intrinsic (_InterlockedExchange64)
#pragma intrinsic (_InterlockedIncrement64)
#pragma intrinsic (_InterlockedDecrement64)
#pragma intrinsic (_InterlockedExchangeAdd64)
#pragma intrinsic (_InterlockedAnd64)
#pragma intrinsic (_InterlockedOr64)

namespace SC_Atomic
{
	uint8 Increment(uint8 volatile& aDst)
	{
		return (uint8)Increment((int8 volatile&)aDst);
	}
	uint16 Increment(uint16 volatile& aDst)
	{
		return (uint16)Increment((int16 volatile&)aDst);
	}
	uint32 Increment(uint32 volatile& aDst)
	{
		return (uint32)Increment((int32 volatile&)aDst);
	}
	uint64 Increment(uint64 volatile& aDst)
	{
		return (uint64)Increment((int64 volatile&)aDst);
	}
	int8 Increment(int8 volatile& aDst)
	{
		return Add(aDst, 1);
	}
	int16 Increment(int16 volatile& aDst)
	{
		return _InterlockedIncrement16((volatile short*)&aDst) - 1;
	}
	int32 Increment(int32 volatile& aDst)
	{
		return _InterlockedIncrement((volatile long*)&aDst) - 1;
	}
	int64 Increment(int64 volatile& aDst)
	{
		return _InterlockedIncrement64((volatile __int64*)&aDst) - 1;
	}

	uint8 Increment_GetNew(uint8 volatile& aDst)
	{
		return (uint8)Increment_GetNew((int8 volatile&)aDst);
	}
	uint16 Increment_GetNew(uint16 volatile& aDst)
	{
		return (uint16)Increment_GetNew((int16 volatile&)aDst);
	}
	uint32 Increment_GetNew(uint32 volatile& aDst)
	{
		return (uint32)Increment_GetNew((int32 volatile&)aDst);
	}
	uint64 Increment_GetNew(uint64 volatile& aDst)
	{
		return (uint64)Increment_GetNew((int64 volatile&)aDst);
	}
	int8 Increment_GetNew(int8 volatile& aDst)
	{
		return Add(aDst, 1) + 1;
	}
	int16 Increment_GetNew(int16 volatile& aDst)
	{
		return _InterlockedIncrement16((volatile short*)&aDst);
	}
	int32 Increment_GetNew(int32 volatile& aDst)
	{
		return _InterlockedIncrement((volatile long*)&aDst);
	}
	int64 Increment_GetNew(int64 volatile& aDst)
	{
		return _InterlockedIncrement64((volatile __int64*)&aDst);
	}

	uint8 Decrement(uint8 volatile& aDst)
	{
		return (uint8)Decrement((int8 volatile&)aDst);
	}
	uint16 Decrement(uint16 volatile& aDst)
	{
		return (uint16)Decrement((int16 volatile&)aDst);
	}
	uint32 Decrement(uint32 volatile& aDst)
	{
		return (uint32)Decrement((int32 volatile&)aDst);
	}
	uint64 Decrement(uint64 volatile& aDst)
	{
		return (uint64)Decrement((int64 volatile&)aDst);
	}
	int8 Decrement(int8 volatile& aDst)
	{
		return Subtract(aDst, 1);
	}
	int16 Decrement(int16 volatile& aDst)
	{
		return _InterlockedDecrement16((volatile short*)&aDst) + 1;
	}
	int32 Decrement(int32 volatile& aDst)
	{
		return _InterlockedDecrement((volatile long*)&aDst) + 1;
	}
	int64 Decrement(int64 volatile& aDst)
	{
		return _InterlockedDecrement64((volatile __int64*)&aDst) + 1;
	}

	uint8 Decrement_GetNew(uint8 volatile& aDst)
	{
		return (uint8)Decrement_GetNew((int8 volatile&)aDst);
	}
	uint16 Decrement_GetNew(uint16 volatile& aDst)
	{
		return (uint16)Decrement_GetNew((int16 volatile&)aDst);
	}
	uint32 Decrement_GetNew(uint32 volatile& aDst)
	{
		return (uint32)Decrement_GetNew((int32 volatile&)aDst);
	}
	uint64 Decrement_GetNew(uint64 volatile& aDst)
	{
		return (uint64)Decrement_GetNew((int64 volatile&)aDst);
	}
	int8 Decrement_GetNew(int8 volatile& aDst)
	{
		return Subtract(aDst, 1) - 1;
	}
	int16 Decrement_GetNew(int16 volatile& aDst)
	{
		return _InterlockedDecrement16((volatile short*)&aDst);
	}
	int32 Decrement_GetNew(int32 volatile& aDst)
	{
		return _InterlockedDecrement((volatile long*)&aDst);
	}
	int64 Decrement_GetNew(int64 volatile& aDst)
	{
		return _InterlockedDecrement64((volatile __int64*)&aDst);
	}

	uint8 Exchange_GetOld(uint8 volatile& aDst, uint8 aValue)
	{
		return (uint8)Exchange_GetOld((int8 volatile&)aDst, (int8)aValue);
	}
	int8 Exchange_GetOld(int8 volatile& aDst, int8 aValue)
	{
		return _InterlockedExchange8((volatile char*)&aDst, aValue);
	}
	uint16 Exchange_GetOld(uint16 volatile& aDst, uint16 aValue)
	{
		return (uint16)Exchange_GetOld((int16 volatile&)aDst, (int16)aValue);
	}
	int16 Exchange_GetOld(int16 volatile& aDst, int16 aValue)
	{
		return _InterlockedExchange16((volatile short*)&aDst, aValue);
	}
	uint32 Exchange_GetOld(uint32 volatile& aDst, uint32 aValue)
	{
		return (uint32)Exchange_GetOld((int32 volatile&)aDst, (int32)aValue);
	}
	int32 Exchange_GetOld(int32 volatile& aDst, int32 aValue)
	{
		return _InterlockedExchange((volatile long*)&aDst, aValue);
	}
	uint64 Exchange_GetOld(uint64 volatile& aDst, uint64 aValue)
	{
		return (uint64)Exchange_GetOld((int64 volatile&)aDst, (int64)aValue);
	}
	int64 Exchange_GetOld(int64 volatile& aDst, int64 aValue)
	{
		return _InterlockedExchange64((volatile __int64*)&aDst, aValue);
	}

	void* ExchangePtr(void* volatile* aDst, void* aExchange)
	{
#if IS_DEBUG
		//if (IsAligned(aDst, alignof(void*)) == false)
		//{
		//	printf("ExchangePtr requires aDst pointer to be aligned to %d bytes.\n", (int)alignof(void*));
		//}
#endif
		return _InterlockedExchangePointer(aDst, aExchange);
	}

	bool CompareExchangePtr(void* volatile* aDst, void* aExchange, void* aComperand)
	{
#if IS_DEBUG
		//if (IsAligned(aDst, alignof(void*)) == false)
		//{
		//	printf("ExchangePtr requires aDst pointer to be aligned to %d bytes.\n", (int)alignof(void*));
		//}
#endif
		return _InterlockedCompareExchangePointer(aDst, aExchange, aComperand) == aComperand;
	}

	uint8 CompareExchange_GetOld(uint8 volatile& aDst, uint8 aValue, uint8 aComperand)
	{
		return (uint8)CompareExchange_GetOld((int8 volatile&)aDst, (int8)aValue, (int8)aComperand);
	}
	bool CompareExchange(uint8 volatile& aDst, uint8 aValue, uint8 aComperand)
	{
		return CompareExchange_GetOld(aDst, aValue, aComperand) == aComperand;
	}
	int8 CompareExchange_GetOld(int8 volatile& aDst, int8 aValue, int8 aComperand)
	{
		return _InterlockedCompareExchange8((volatile char*)&aDst, aValue, aComperand);
	}
	bool CompareExchange(int8 volatile& aDst, int8 aValue, int8 aComperand)
	{
		return CompareExchange_GetOld(aDst, aValue, aComperand) == aComperand;
	}
	uint16 CompareExchange_GetOld(uint16 volatile& aDst, uint16 aValue, uint16 aComperand)
	{
		return (uint16)CompareExchange_GetOld((int16 volatile&)aDst, (int16)aValue, (int16)aComperand);
	}
	bool CompareExchange(uint16 volatile& aDst, uint16 aValue, uint16 aComperand)
	{
		return CompareExchange_GetOld(aDst, aValue, aComperand) == aComperand;
	}
	int16 CompareExchange_GetOld(int16 volatile& aDst, int16 aValue, int16 aComperand)
	{
		return _InterlockedCompareExchange16((volatile short*)&aDst, aValue, aComperand);
	}
	bool CompareExchange(int16 volatile& aDst, int16 aValue, int16 aComperand)
	{
		return CompareExchange_GetOld(aDst, aValue, aComperand) == aComperand;
	}
	uint32 CompareExchange_GetOld(uint32 volatile& aDst, uint32 aValue, uint32 aComperand)
	{
		return (uint32)CompareExchange_GetOld((int32 volatile&)aDst, (int32)aValue, (int32)aComperand);
	}
	bool CompareExchange(uint32 volatile& aDst, uint32 aValue, uint32 aComperand)
	{
		return CompareExchange_GetOld(aDst, aValue, aComperand) == aComperand;
	}
	int32 CompareExchange_GetOld(int32 volatile& aDst, int32 aValue, int32 aComperand)
	{
		return _InterlockedCompareExchange((volatile long*)&aDst, aValue, aComperand);
	}
	bool CompareExchange(int32 volatile& aDst, int32 aValue, int32 aComperand)
	{
		return CompareExchange_GetOld(aDst, aValue, aComperand) == aComperand;
	}
	uint64 CompareExchange_GetOld(uint64 volatile& aDst, uint64 aValue, uint64 aComperand)
	{
		return (uint64)CompareExchange_GetOld((int64 volatile&)aDst, (int64)aValue, (int64)aComperand);
	}
	bool CompareExchange(uint64 volatile& aDst, uint64 aValue, uint64 aComperand)
	{
		return CompareExchange_GetOld(aDst, aValue, aComperand) == aComperand;
	}
	int64 CompareExchange_GetOld(int64 volatile& aDst, int64 aValue, int64 aComperand)
	{
		return _InterlockedCompareExchange64((volatile __int64*)&aDst, aValue, aComperand);
	}
	bool CompareExchange(int64 volatile& aDst, int64 aValue, int64 aComperand)
	{
		return CompareExchange_GetOld(aDst, aValue, aComperand) == aComperand;
	}

	uint8 Add(uint8 volatile& aDst, uint8 aValue)
	{
		return (uint8)Add((int8 volatile&)aDst, (int8)aValue);
	}
	int8 Add(int8 volatile& aDst, int8 aValue)
	{
		return _InterlockedExchangeAdd8((volatile char*)&aDst, (char)aValue);
	}
	uint16 Add(uint16 volatile& aDst, uint16 aValue)
	{
		return (uint16)Add((int16 volatile&)aDst, (int16)aValue);
	}
	int16 Add(int16 volatile& aDst, int16 aValue)
	{
		return _InterlockedExchangeAdd16((volatile short*)&aDst, (short)aValue);
	}
	uint32 Add(uint32 volatile& aDst, uint32 aValue)
	{
		return (uint32)Add((int32 volatile&)aDst, (int32)aValue);
	}
	int32 Add(int32 volatile& aDst, int32 aValue)
	{
		return _InterlockedExchangeAdd((volatile long*)&aDst, (long)aValue);
	}
	uint64 Add(uint64 volatile& aDst, uint64 aValue)
	{
		return (uint64)Add((int64 volatile&)aDst, (int64)aValue);
	}
	int64 Add(int64 volatile& aDst, int64 aValue)
	{
		return _InterlockedExchangeAdd64((volatile __int64*)&aDst, (__int64)aValue);
	}

	uint8 Subtract(uint8 volatile& aDst, uint8 aValue)
	{
		return (uint8)Subtract((int8 volatile&)aDst, -(int8)aValue);
	}
	int8 Subtract(int8 volatile& aDst, int8 aValue)
	{
		return Add(aDst, -aValue);
	}
	uint16 Subtract(uint16 volatile& aDst, uint16 aValue)
	{
		return (uint16)Subtract((int16 volatile&)aDst, -(int16)aValue);
	}
	int16 Subtract(int16 volatile& aDst, int16 aValue)
	{
		return Add(aDst, -aValue);
	}
	uint32 Subtract(uint32 volatile& aDst, uint32 aValue)
	{
		return (uint32)Subtract((int32 volatile&)aDst, -(int32)aValue);
	}
	int32 Subtract(int32 volatile& aDst, int32 aValue)
	{
		return Add(aDst, -aValue);
	}
	uint64 Subtract(uint64 volatile& aDst, uint64 aValue)
	{
		return (uint64)Subtract((int64 volatile&)aDst, -(int64)aValue);
	}
	int64 Subtract(int64 volatile& aDst, int64 aValue)
	{
		return Add(aDst, -aValue);
	}

	uint8 And(uint8 volatile& aDst, uint8 aValue)
	{
		return (uint8)And((int8 volatile&)aDst, (int8)aValue);
	}
	int8 And(int8 volatile& aDst, int8 aValue)
	{
		return _InterlockedAnd8((volatile char*)&aDst, (char)aValue);
	}
	uint16 And(uint16 volatile& aDst, uint16 aValue)
	{
		return (uint16)And((int16 volatile&)aDst, (int16)aValue);
	}
	int16 And(int16 volatile& aDst, int16 aValue)
	{
		return _InterlockedAnd16((volatile short*)&aDst, (short)aValue);
	}
	uint32 And(uint32 volatile& aDst, uint32 aValue)
	{
		return (uint32)And((int32 volatile&)aDst, (int32)aValue);
	}
	int32 And(int32 volatile& aDst, int32 aValue)
	{
		return _InterlockedAnd((volatile long*)&aDst, (long)aValue);
	}
	uint64 And(uint64 volatile& aDst, uint64 aValue)
	{
		return (uint64)And((int64 volatile&)aDst, (int64)aValue);
	}
	int64 And(int64 volatile& aDst, int64 aValue)
	{
		return _InterlockedAnd64((volatile __int64*)&aDst, (__int64)aValue);
	}

	uint8 Or(uint8 volatile& aDst, uint8 aValue)
	{
		return (uint8)Or((int8 volatile&)aDst, (int8)aValue);
	}
	int8 Or(int8 volatile& aDst, int8 aValue)
	{
		return _InterlockedOr8((volatile char*)&aDst, (char)aValue);
	}
	uint16 Or(uint16 volatile& aDst, uint16 aValue)
	{
		return (uint16)Or((int16 volatile&)aDst, (int16)aValue);
	}
	int16 Or(int16 volatile& aDst, int16 aValue)
	{
		return _InterlockedOr16((volatile short*)&aDst, (short)aValue);
	}
	uint32 Or(uint32 volatile& aDst, uint32 aValue)
	{
		return (uint32)Or((int32 volatile&)aDst, (int32)aValue);
	}
	int32 Or(int32 volatile& aDst, int32 aValue)
	{
		return _InterlockedOr((volatile long*)&aDst, (long)aValue);
	}
	uint64 Or(uint64 volatile& aDst, uint64 aValue)
	{
		return (uint64)Or((int64 volatile&)aDst, (int64)aValue);
	}
	int64 Or(int64 volatile& aDst, int64 aValue)
	{
		return _InterlockedOr64((volatile __int64*)&aDst, (__int64)aValue);
	}
}
#endif