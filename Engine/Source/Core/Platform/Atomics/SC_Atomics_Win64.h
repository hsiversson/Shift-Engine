#pragma once

#if IS_WINDOWS_PLATFORM
namespace SC_Atomic
{
	// Increment
	uint8 Increment(uint8 volatile& aDst);
	uint16 Increment(uint16 volatile& aDst);
	uint32 Increment(uint32 volatile& aDst);
	uint64 Increment(uint64 volatile& aDst);

	int8 Increment(int8 volatile& aDst);
	int16 Increment(int16 volatile& aDst);
	int32 Increment(int32 volatile& aDst);
	int64 Increment(int64 volatile& aDst);

	uint8 Increment_GetNew(uint8 volatile& aDst);
	uint16 Increment_GetNew(uint16 volatile& aDst);
	uint32 Increment_GetNew(uint32 volatile& aDst);
	uint64 Increment_GetNew(uint64 volatile& aDst);

	int8 Increment_GetNew(int8 volatile& aDst);
	int16 Increment_GetNew(int16 volatile& aDst);
	int32 Increment_GetNew(int32 volatile& aDst);
	int64 Increment_GetNew(int64 volatile& aDst);

	// Decrement
	uint8 Decrement(uint8 volatile& aDst);
	uint16 Decrement(uint16 volatile& aDst);
	uint32 Decrement(uint32 volatile& aDst);
	uint64 Decrement(uint64 volatile& aDst);

	int8 Decrement(int8 volatile& aDst);
	int16 Decrement(int16 volatile& aDst);
	int32 Decrement(int32 volatile& aDst);
	int64 Decrement(int64 volatile& aDst);

	uint8 Decrement_GetNew(uint8 volatile& aDst);
	uint16 Decrement_GetNew(uint16 volatile& aDst);
	uint32 Decrement_GetNew(uint32 volatile& aDst);
	uint64 Decrement_GetNew(uint64 volatile& aDst);

	int8 Decrement_GetNew(int8 volatile& aDst);
	int16 Decrement_GetNew(int16 volatile& aDst);
	int32 Decrement_GetNew(int32 volatile& aDst);
	int64 Decrement_GetNew(int64 volatile& aDst);

	// aExchange
	uint8  Exchange_GetOld(uint8  volatile& aDst, uint8  aValue);
	int8   Exchange_GetOld(int8   volatile& aDst, int8   aValue);
	uint16 Exchange_GetOld(uint16 volatile& aDst, uint16 aValue);
	int16  Exchange_GetOld(int16  volatile& aDst, int16  aValue);
	uint32 Exchange_GetOld(uint32 volatile& aDst, uint32 aValue);
	int32  Exchange_GetOld(int32  volatile& aDst, int32  aValue);
	uint64 Exchange_GetOld(uint64 volatile& aDst, uint64 aValue);
	int64  Exchange_GetOld(int64  volatile& aDst, int64  aValue);
	void* ExchangePtr(void* volatile* aDst, void* aExchange);
	bool CompareExchangePtr(void* volatile* aDst, void* aExchange, void* aComperand);

	// Compare aExchange
	uint8 CompareExchange_GetOld(uint8 volatile& aDst, uint8 aValue, uint8 aComperand);
	bool CompareExchange(uint8 volatile& aDst, uint8 aValue, uint8 aComperand);
	int8 CompareExchange_GetOld(int8 volatile& aDst, int8 aValue, int8 aComperand);
	bool CompareExchange(int8 volatile& aDst, int8 aValue, int8 aComperand);

	uint16 CompareExchange_GetOld(uint16 volatile& aDst, uint16 aValue, uint16 aComperand);
	bool CompareExchange(uint16 volatile& aDst, uint16 aValue, uint16 aComperand);
	int16 CompareExchange_GetOld(int16 volatile& aDst, int16 aValue, int16 aComperand);
	bool CompareExchange(int16 volatile& aDst, int16 aValue, int16 aComperand);

	uint32 CompareExchange_GetOld(uint32 volatile& aDst, uint32 aValue, uint32 aComperand);
	bool CompareExchange(uint32 volatile& aDst, uint32 aValue, uint32 aComperand);
	int32 CompareExchange_GetOld(int32 volatile& aDst, int32 aValue, int32 aComperand);
	bool CompareExchange(int32 volatile& aDst, int32 aValue, int32 aComperand);

	uint64 CompareExchange_GetOld(uint64 volatile& aDst, uint64 aValue, uint64 aComperand);
	bool CompareExchange(uint64 volatile& aDst, uint64 aValue, uint64 aComperand);
	int64 CompareExchange_GetOld(int64 volatile& aDst, int64 aValue, int64 aComperand);
	bool CompareExchange(int64 volatile& aDst, int64 aValue, int64 aComperand);

	// Add
	uint8  Add(uint8  volatile& aDst, uint8  aValue);
	int8   Add(int8   volatile& aDst, int8   aValue);
	uint16 Add(uint16 volatile& aDst, uint16 aValue);
	int16  Add(int16  volatile& aDst, int16  aValue);
	uint32 Add(uint32 volatile& aDst, uint32 aValue);
	int32  Add(int32  volatile& aDst, int32  aValue);
	uint64 Add(uint64 volatile& aDst, uint64 aValue);
	int64  Add(int64  volatile& aDst, int64  aValue);

	// Subtract
	uint8  Subtract(uint8  volatile& aDst, uint8  aValue);
	int8   Subtract(int8   volatile& aDst, int8   aValue);
	uint16 Subtract(uint16 volatile& aDst, uint16 aValue);
	int16  Subtract(int16  volatile& aDst, int16  aValue);
	uint32 Subtract(uint32 volatile& aDst, uint32 aValue);
	int32  Subtract(int32  volatile& aDst, int32  aValue);
	uint64 Subtract(uint64 volatile& aDst, uint64 aValue);
	int64  Subtract(int64  volatile& aDst, int64  aValue);

	// Bitwise AND
	uint8  And(uint8  volatile& aDst, uint8  aValue);
	int8   And(int8   volatile& aDst, int8   aValue);
	uint16 And(uint16 volatile& aDst, uint16 aValue);
	int16  And(int16  volatile& aDst, int16  aValue);
	uint32 And(uint32 volatile& aDst, uint32 aValue);
	int32  And(int32  volatile& aDst, int32  aValue);
	uint64 And(uint64 volatile& aDst, uint64 aValue);
	int64  And(int64  volatile& aDst, int64  aValue);

	// Bitwise OR
	uint8  Or(uint8  volatile& aDst, uint8  aValue);
	int8   Or(int8   volatile& aDst, int8   aValue);
	uint16 Or(uint16 volatile& aDst, uint16 aValue);
	int16  Or(int16  volatile& aDst, int16  aValue);
	uint32 Or(uint32 volatile& aDst, uint32 aValue);
	int32  Or(int32  volatile& aDst, int32  aValue);
	uint64 Or(uint64 volatile& aDst, uint64 aValue);
	int64  Or(int64  volatile& aDst, int64  aValue);
}

#endif