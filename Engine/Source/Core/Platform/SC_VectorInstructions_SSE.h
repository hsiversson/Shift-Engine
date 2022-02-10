#pragma once

#ifndef SC_VECTOR_INSTRUCTIONS_HEADER
#error	Include "SC_VectorInstructions.h" instead.
#endif

#include <immintrin.h> // AVX2 Requirement

namespace SC_VectorInstructions
{
	using VectorRegister = __m128;


	inline VectorRegister Load(float aValue)
	{
		return _mm_setr_ps(aValue, aValue, aValue, aValue);
	}

	inline VectorRegister Load(float aX, float aY, float aZ, float aW)
	{
		return _mm_setr_ps(aX, aY, aZ, aW);
	}

	inline VectorRegister LoadAligned(const float* aVec)
	{
		return _mm_load_ps(aVec);
	}

	inline void GetVector(VectorRegister aVec, float& aOutX, float& aOutY, float& aOutZ, float& aOutW)
	{
		aOutX = (((float*)&(aVec))[0]);
		aOutY = (((float*)&(aVec))[1]);
		aOutZ = (((float*)&(aVec))[2]);
		aOutW = (((float*)&(aVec))[3]);
	}

	inline VectorRegister Add(const VectorRegister& aVec1, const VectorRegister& aVec2)
	{
		return _mm_add_ps(aVec1, aVec2);
	}

	inline VectorRegister Subtract(const VectorRegister& aVec1, const VectorRegister& aVec2)
	{
		return _mm_sub_ps(aVec1, aVec2);
	}
	
	inline VectorRegister Multiply(const VectorRegister& aVec1, const VectorRegister& aVec2)
	{
		return _mm_mul_ps(aVec1, aVec2);
	}

	inline VectorRegister Divide(const VectorRegister& aVec1, const VectorRegister& aVec2)
	{
		return _mm_div_ps(aVec1, aVec2);
	}

	inline VectorRegister MultiplyAdd(const VectorRegister& aVec1, const VectorRegister& aVec2, const VectorRegister& aVec3)
	{
		return _mm_add_ps(_mm_mul_ps(aVec1, aVec2), aVec3);
	}

	inline VectorRegister Reciprocal(const VectorRegister& aVec)
	{
		return _mm_rcp_ps(aVec);
	}

	inline VectorRegister ReciprocalSqrt(const VectorRegister& aVec)
	{
		return _mm_rsqrt_ps(aVec);
	}

	inline VectorRegister Min(const VectorRegister& aVec1, const VectorRegister& aVec2)
	{
		return _mm_min_ps(aVec1, aVec2);
	}

	inline VectorRegister Max(const VectorRegister& aVec1, const VectorRegister& aVec2)
	{
		return _mm_max_ps(aVec1, aVec2);
	}
}