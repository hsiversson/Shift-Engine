#pragma once
#include "Types/SC_TypeDefines.h"
#include "Types/SC_TypeTraits.h"

#include <new>

void SC_Memcpy(void* aDst, const void* aSrc, uint64 aSize);
void SC_Memmove(void* aDst, void* aSrc, uint64 aSize);
void SC_Memset(void* aDst, int aValue, uint64 aSize);
void SC_ZeroMemory(void* aDst, uint64 aSize);

inline void SC_MemcpyRect(void* aDst, uint32 aDstBytesPerLine, const void* aSrc, uint32 aSrcBytesPerLine, uint32 aNumLines, uint32 aCopyBytesPerLine)
{
	if (aDstBytesPerLine == aSrcBytesPerLine && aCopyBytesPerLine == aDstBytesPerLine)
	{
		SC_Memcpy(aDst, aSrc, uint64(aDstBytesPerLine) * uint64(aNumLines));
	}
	else
	{
		uint8* dst = static_cast<uint8*>(aDst);
		const uint8* src = static_cast<const uint8*>(aSrc);

		for (uint32 i = 0; i < aNumLines; ++i)
		{
			SC_Memcpy(dst + (uint64(aDstBytesPerLine) * uint64(i)), src + (uint64(aSrcBytesPerLine) * uint64(i)), aCopyBytesPerLine);
		}
	}
}

inline void SC_MemcpyRect(void* aDst, uint32 aDstBytesPerLine, const void* aSrc, uint32 aSrcBytesPerLine, uint32 aNumLines)
{
	uint32 minCpyBytesPerLine = (aDstBytesPerLine < aSrcBytesPerLine) ? aDstBytesPerLine : aSrcBytesPerLine;
	SC_MemcpyRect(aDst, aDstBytesPerLine, aSrc, aSrcBytesPerLine, aNumLines, minCpyBytesPerLine);
}

inline uint32 SC_Align(uint32 aValue, uint32 aAlignment)
{
	return (aValue + aAlignment - 1) & ~(aAlignment - 1);
}

inline uint64 SC_Align(uint64 aValue, uint64 aAlignment)
{
	return (aValue + aAlignment - 1) & ~(aAlignment - 1);
}

template<typename BufferType, typename ValueType>
inline void SC_Fill(BufferType* aBuffer, uint32 aCount, ValueType aValue)
{
	for (uint32 i = 0; i < aCount; ++i)
		aBuffer[i] = static_cast<BufferType>(aValue);
}

template<class Type>
inline void SC_Construct(const Type* aDst)
{
	new (const_cast<typename SC_RemoveConst<Type>::Type*>(aDst)) Type();
}

template<class Type>
inline void SC_Construct(const Type* aDst, uint32 aCount)
{
	for (uint32 i = 0; i < aCount; ++i)
		new (const_cast<typename SC_RemoveConst<Type>::Type*>(aDst + i)) Type();
}

template<class Type>
inline void SC_Destruct(const Type* aDst)
{
	if (aDst)
		aDst->~Type();
}

template<class Type>
inline void SC_Destruct(const Type* aDst, uint32 aCount)
{
	if (!aDst)
		return;

	for (int32 i = (aCount - 1); i >= 0; --i)
		(aDst + i)->~Type();
}

template<class Type>
inline void SC_CopyConstruct(const Type* aDst, const Type& aSrc)
{
	new (const_cast<typename SC_RemoveConst<Type>::Type*>(aDst)) Type(aSrc);
}

template<class Type>
inline void SC_CopyConstruct(const Type* aDst, const Type* aSrc, uint32 aCount)
{
	for (uint32 i = 0; i < aCount; ++i)
		new (const_cast<typename SC_RemoveConst<Type>::Type*>(aDst + i)) Type(aSrc[i]);
}

template<class Type>
inline void SC_MoveConstruct(Type* aDst, Type* aSrc, uint32 aCount)
{
	for (uint32 i = 0; i < aCount; ++i)
		new (static_cast<void*>(aDst + i)) Type(SC_Move(aSrc[i]));
}