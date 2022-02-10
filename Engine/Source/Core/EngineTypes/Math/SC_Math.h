#pragma once

namespace SC_Math
{
	static constexpr float PI = (3.1415926535897932f);

	float Sin(float aValue);
	float Asin(float aValue);
	float Cos(float aValue);
	float Acos(float aValue);
	float Tan(float aValue);
	float Atan(float aValue);
	float Atan2(float aX, float aY);
	float Sqrt(float aValue);
	float InvSqrt(float aValue);
	float Pow(float aValue, float aExp);
	float Square(float aValue);
	float Log(float aValue);
	float Log2(float aValue);
	float Abs(float aValue);

	float Floor(float aValue);
	float Ceil(float aValue);
	float Frac(float aValue);

	inline void SinCos(float* aOutSin, float* aOutCos, float aValue)
	{
		*aOutSin = Sin(aValue);
		*aOutCos = Cos(aValue);
	}

	/* Converts radians to degrees. */
	template<class T>
	inline auto RadiansToDegrees(T const& aRadVal) -> decltype(aRadVal* (180.f / PI))
	{
		return aRadVal * (180.f / PI);
	}

	/* Converts degrees to radians. */
	template<class T>
	inline auto DegreesToRadians(T const& aDegVal) -> decltype(aDegVal* (PI / 180.f))
	{
		return aDegVal * (PI / 180.f);
	}

	template<class T>
	inline T Lerp(T aA, T aB, T aValue)
	{
		return (aA * ((T)1 - aValue) + (aB * aValue));
	}

	template<class T>
	inline T FastLerp(T aA, T aB, T aValue)
	{
		return aA + aValue * (aB - aA);
	}
}

template<class T>
class SC_TVector2;
template<class T>
class SC_TVector3;
template<class T>
class SC_TVector4;

#include "SIMD/SC_VectorInstructions.h"
#include "SC_Vector2.h"
#include "SC_Vector.h"
#include "SC_Vector4.h"
#include "SC_Matrix.h"
#include "SC_MatrixTypes.h"
#include "SC_Quaternion.h"
#include "Geometry/SC_Plane.h"
#include "Geometry/SC_AABB.h"
#include "Geometry/SC_Sphere.h"

inline SC_Vector operator*(const SC_Vector& aVector, const SC_Matrix& aMatrix)
{
	const SC_Vector4 xAxis = aMatrix.mVectorX;
	const SC_Vector4 yAxis = aMatrix.mVectorY;
	const SC_Vector4 zAxis = aMatrix.mVectorZ;
	const SC_Vector4 translation = aMatrix.GetTranslation();
	return SC_Vector(
		(aVector.x * xAxis.x + aVector.y * yAxis.x + aVector.z * zAxis.x + translation.x),
		(aVector.x * xAxis.y + aVector.y * yAxis.y + aVector.z * zAxis.y + translation.y),
		(aVector.x * xAxis.z + aVector.y * yAxis.z + aVector.z * zAxis.z + translation.z));
}

inline SC_Vector& operator*=(SC_Vector& aVector, const SC_Matrix& aMatrix)
{
	aVector = aVector * aMatrix;
	return aVector;
}

inline SC_Vector operator*(const SC_Vector& aVector, const SC_Quaternion& aQuat)
{
	SC_Vector qV(aQuat.x, aQuat.y, aQuat.z);
	float qS = aQuat.w;

	SC_Vector rotated = qV*(2.0f*qV.Dot(aVector)) + aVector*(qS*qS - qV.Dot(qV)) + qV.Cross(aVector)*2.0f*qS;
	return rotated;
}

inline SC_Vector& operator*=(SC_Vector& aVector, const SC_Quaternion& aQuat)
{
	aVector = aVector * aQuat;
	return aVector;
}

inline SC_Vector4 operator*(const SC_Vector4& aVector, const SC_Matrix& aMatrix)
{
	return SC_Vector4(
		(aVector.x * aMatrix.m[0] + aVector.y * aMatrix.m[4] + aVector.z * aMatrix.m[8] + aVector.w * aMatrix.m[12]),
		(aVector.x * aMatrix.m[1] + aVector.y * aMatrix.m[5] + aVector.z * aMatrix.m[9] + aVector.w * aMatrix.m[13]),
		(aVector.x * aMatrix.m[2] + aVector.y * aMatrix.m[6] + aVector.z * aMatrix.m[10] + aVector.w * aMatrix.m[14]),
		(aVector.x * aMatrix.m[3] + aVector.y * aMatrix.m[7] + aVector.z * aMatrix.m[11] + aVector.w * aMatrix.m[15]));
}

inline SC_Vector4& operator*=(SC_Vector4& aVector, const SC_Matrix& aMatrix)
{
	aVector = aVector * aMatrix;
	return aVector;
}