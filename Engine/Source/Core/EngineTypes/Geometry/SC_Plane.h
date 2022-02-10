#pragma once
#include "Platform/Debugging/SC_Assert.h"

class SC_Plane
{
public:
	SC_Vector mNormal;
	float mDistance;

public:
	inline void Normalize() 
	{
		float L2 = mNormal.Length2();
		assert(L2 >= 0);
		float normalLength = SC_Math::Sqrt(L2);
		float T = normalLength > 0.f ? (1.0f / normalLength) : 0.f;
		mNormal.x *= T;
		mNormal.y *= T;
		mNormal.z *= T;
		mDistance *= T;
	}

	float DistanceToPlane(const SC_Vector& aPoint) const
	{
		return mNormal.Dot(aPoint) + mDistance;
	}
};