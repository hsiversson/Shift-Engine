#pragma once
#include "Types/SC_TypeTraits.h"
#include "Debugging/SC_Assert.h"

//-------------------------------------------------------------//
// Min / Max

template <class Type1, class Type2>
inline Type1 SC_Min(const Type1& aFirst, const Type2& aSecond)
{
	return (aFirst < static_cast<Type1>(aSecond)) ? aFirst : static_cast<Type1>(aSecond);
}

template <class Type1, class Type2>
inline Type1 SC_Max(const Type1& aFirst, const Type2& aSecond)
{
	return (aFirst < static_cast<Type1>(aSecond)) ? static_cast<Type1>(aSecond) : aFirst;
}

template<typename Type>
inline Type SC_Max(const Type& aFirst, const Type& aSecond, const Type& aThird)
{
	Type temp = SC_Max(aFirst, aSecond);
	return SC_Max(temp, aThird);
}

template<typename Type>
inline Type SC_Clamp(const Type& aValue, const Type& aMin, const Type& aMax)
{
	if (aValue < aMin)
		return aMin;
	else if (aMax < aValue)
		return aMax;
	else
		return aValue;
}


//-------------------------------------------------------------//
// Move
template<typename T>
inline void SC_Exchange(T& aDst, T& aSrc)
{
	aDst.~T();
	new(&aDst) T(SC_MoveStrict(aSrc));
}

template <typename T>
inline typename SC_RemoveReference<T>::Type&& SC_Move(T&& aObj)
{
	using CastType = typename SC_RemoveReference<T>::Type;
	return (CastType&&)aObj;
}

template<typename T>
inline typename SC_RemoveReference<T>::Type&& SC_MoveStrict(T&& aObj)
{
	using CastType = typename SC_RemoveReference<T>::Type;
	static_assert(SC_IsLeftValueReference<T>::mValue, "SC_MoveStrict called on an rvalue");
	static_assert(!SC_AreTypesEqual<CastType&, const CastType&>::mValue, "SC_MoveStrict called on a const object");

	return (CastType&&)(aObj);
}

template<typename T>
inline void SC_Swap(T& aObj1, T& aObj2)
{
	T temp = SC_Move(aObj1);
	aObj1 = SC_Move(aObj2);
	aObj2 = SC_Move(temp);
}

//-------------------------------------------------------------//
//	Forward an rvalue as an rvalue 

template<typename T>
inline T&& SC_Forward(typename SC_RemoveReference<T>::Type&& aValue)
{
	static_assert(!SC_IsLeftValueReference<T>::mValue && "Invalid forwarding");
	return static_cast<T&&>(aValue);
}

//-------------------------------------------------------------//
//	Power of Two

static inline bool SC_IsPow2(const uint32 aValue) { return (aValue && !(aValue & (aValue - 1))); }
static inline bool SC_IsPow2(const float aValue) { return SC_IsPow2(uint32(aValue)); }

static inline uint32 SC_GetNextPow2(uint32 aValue)
{
	static_assert(sizeof(aValue) == 4);
	aValue--;
	aValue |= (aValue >> 1);
	aValue |= (aValue >> 2);
	aValue |= (aValue >> 4);
	aValue |= (aValue >> 8);
	aValue |= (aValue >> 16);
	return (aValue + 1);
}

static inline uint64 SC_GetNextPow2(uint64 aValue)
{
	static_assert(sizeof(aValue) == 8);
	aValue--;
	aValue |= (aValue >> 1);
	aValue |= (aValue >> 2);
	aValue |= (aValue >> 4);
	aValue |= (aValue >> 8);
	aValue |= (aValue >> 16);
	aValue |= (aValue >> 32);
	return (aValue + 1);
}