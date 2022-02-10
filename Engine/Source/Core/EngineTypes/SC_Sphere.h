#pragma once
#include "SC_Vector.h"
#include "SC_AABB.h"

class SC_Sphere
{
public:
	SC_Sphere() : mCenter(SC_Vector::ZeroVector()), mRadius(1.0f) {}
	SC_Sphere(const SC_Vector& aCenter, float aRadius) : mCenter(aCenter), mRadius(aRadius) {}
	SC_Sphere(const SC_Sphere& aOther) : mCenter(aOther.mCenter), mRadius(aOther.mRadius) {}
	SC_Sphere(const SC_AABB& aAABB)
	{
		const SC_Vector& point0 = aAABB.mMin;
		const SC_Vector& point1 = aAABB.mMax;
		mCenter = (point0 + point1) * 0.5f;
		mRadius = (point1 - point0).Length() * 0.5f;
	}

	~SC_Sphere() {}

	inline bool IsInside(const SC_Vector& aPoint) const
	{
		if ((mCenter - aPoint).Length2() < (mRadius * mRadius))
		{
			return true;
		}

		return false;
	}

	SC_Vector mCenter;
	float mRadius;
};