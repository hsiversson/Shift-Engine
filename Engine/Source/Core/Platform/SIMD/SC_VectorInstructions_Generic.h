#pragma once

#ifndef SC_VECTOR_INSTRUCTIONS_HEADER
#error	Include "SC_VectorInstructions.h" instead.
#endif

namespace SC_VectorInstructions
{
	struct VectorRegister
	{
		float v[4];
	};

	inline VectorRegister Load(float aX, float aY, float aZ, float aW)
	{
		VectorRegister vec;
		vec.v[0] = aX;
		vec.v[1] = aY;
		vec.v[2] = aZ;
		vec.v[3] = aW;
		return vec;
	}

	inline VectorRegister LoadAligned(const float* aVec)
	{
		VectorRegister vec;
		vec.v[0] = aVec[0];
		vec.v[1] = aVec[1];
		vec.v[2] = aVec[2];
		vec.v[3] = aVec[3];
		return vec;
	}

	inline VectorRegister Add(const VectorRegister& aVec1, const VectorRegister& aVec2)
	{
		VectorRegister newVec;
		newVec.v[0] = aVec1.v[0] + aVec2.v[0];
		newVec.v[1] = aVec1.v[1] + aVec2.v[1];
		newVec.v[2] = aVec1.v[2] + aVec2.v[2];
		newVec.v[3] = aVec1.v[3] + aVec2.v[3];
		return newVec;
	}

	inline VectorRegister Subtract(const VectorRegister& aVec1, const VectorRegister& aVec2)
	{
		VectorRegister newVec;
		newVec.v[0] = aVec1.v[0] - aVec2.v[0];
		newVec.v[1] = aVec1.v[1] - aVec2.v[1];
		newVec.v[2] = aVec1.v[2] - aVec2.v[2];
		newVec.v[3] = aVec1.v[3] - aVec2.v[3];
		return newVec;
	}

	inline VectorRegister Multiply(const VectorRegister& aVec1, const VectorRegister& aVec2)
	{
		VectorRegister newVec;
		newVec.v[0] = aVec1.v[0] * aVec2.v[0];
		newVec.v[1] = aVec1.v[1] * aVec2.v[1];
		newVec.v[2] = aVec1.v[2] * aVec2.v[2];
		newVec.v[3] = aVec1.v[3] * aVec2.v[3];
		return newVec;
	}

	inline VectorRegister Divide(const VectorRegister& aVec1, const VectorRegister& aVec2)
	{
		VectorRegister newVec;
		newVec.v[0] = aVec1.v[0] / aVec2.v[0];
		newVec.v[1] = aVec1.v[1] / aVec2.v[1];
		newVec.v[2] = aVec1.v[2] / aVec2.v[2];
		newVec.v[3] = aVec1.v[3] / aVec2.v[3];
		return newVec;
	}

	inline VectorRegister MultiplyAdd(const VectorRegister& aVec1, const VectorRegister& aVec2, const VectorRegister& aVec3)
	{
		return Add(Multiply(aVec1, aVec2), aVec3);
	}

	inline VectorRegister Reciprocal(const VectorRegister& aVec)
	{
		VectorRegister newVec;
		newVec.v[0] = 1.f / aVec.v[0];
		newVec.v[1] = 1.f / aVec.v[1];
		newVec.v[2] = 1.f / aVec.v[2];
		newVec.v[3] = 1.f / aVec.v[3];

		// TODO: HANDLE DIVISION BY 0

		return newVec;
	}

	inline VectorRegister ReciprocalSqrt(const VectorRegister& aVec)
	{
		return aVec;
	}

	inline VectorRegister Min(const VectorRegister& aVec1, const VectorRegister& aVec2)
	{
		VectorRegister newVec;
		newVec.v[0] = (aVec1.v[0] < aVec2.v[0]) ? aVec1.v[0] : aVec2.v[0];
		newVec.v[1] = (aVec1.v[1] < aVec2.v[1]) ? aVec1.v[1] : aVec2.v[1];
		newVec.v[2] = (aVec1.v[2] < aVec2.v[2]) ? aVec1.v[2] : aVec2.v[2];
		newVec.v[3] = (aVec1.v[3] < aVec2.v[3]) ? aVec1.v[3] : aVec2.v[3];
		return newVec;
	}

	inline VectorRegister Max(const VectorRegister& aVec1, const VectorRegister& aVec2)
	{
		VectorRegister newVec;
		newVec.v[0] = (aVec1.v[0] > aVec2.v[0]) ? aVec1.v[0] : aVec2.v[0];
		newVec.v[1] = (aVec1.v[1] > aVec2.v[1]) ? aVec1.v[1] : aVec2.v[1];
		newVec.v[2] = (aVec1.v[2] > aVec2.v[2]) ? aVec1.v[2] : aVec2.v[2];
		newVec.v[3] = (aVec1.v[3] > aVec2.v[3]) ? aVec1.v[3] : aVec2.v[3];
		return newVec;
	}
}