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
		SC_Vector4 v(mNormal, mDistance);
		v.Normalize();

		mNormal = v.XYZ();
		mDistance = v.w;
	}

	float DistanceToPlane(const SC_Vector& aPoint) const
	{
		return mNormal.Dot(aPoint) + mDistance;
	}
};