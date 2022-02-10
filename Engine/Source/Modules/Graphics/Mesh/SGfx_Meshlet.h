#pragma once
#include "Platform/Types/SC_TypeDefines.h"

struct SGfx_Meshlet
{
	uint32 mVertexCount;
	uint32 mVertexOffset;
	uint32 mPrimitiveCount;
	uint32 mPrimitiveOffset;
};

struct SGfx_PackedPrimitiveTriangle
{
	SGfx_PackedPrimitiveTriangle() : mIndex0(SC_UINT32_MAX), mIndex1(SC_UINT32_MAX), mIndex2(SC_UINT32_MAX), __pad(0) {}
	SGfx_PackedPrimitiveTriangle(uint32 aIndex0, uint32 aIndex1, uint32 aIndex2) : mIndex0(aIndex0), mIndex1(aIndex1), mIndex2(aIndex2), __pad(0) {}

	SC_IntVector Unpack() const
	{
		return SC_IntVector(mIndex0 & 0x3FF, mIndex1 & 0x3FF, mIndex2 & 0x3FF);
	}

	uint32 mIndex0 : 10;
	uint32 mIndex1 : 10;
	uint32 mIndex2 : 10;
private:
	uint32 __pad : 2;
};