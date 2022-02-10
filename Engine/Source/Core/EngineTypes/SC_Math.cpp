
#include <cmath>
#include <cfloat>

#include "SC_Math.h"

namespace SC_Math
{
	float Sin(float aValue)
	{
		return sinf(aValue);
	}
	float Asin(float aValue)
	{
		return asinf(aValue);
	}
	float Cos(float aValue)
	{
		return cosf(aValue);
	}
	float Acos(float aValue)
	{
		return acosf(aValue);
	}
	float Tan(float aValue)
	{
		return tanf(aValue);
	}
	float Atan(float aValue)
	{
		return atanf(aValue);
	}
	float Atan2(float aX, float aY)
	{
		return atan2f(aX, aY);
	}
	float Sqrt(float aValue)
	{
		return sqrtf(aValue);
	}
	float InvSqrt(float aValue)
	{
		return (1.0f / sqrtf(aValue));
	}
	float Pow(float aValue, float aExp)
	{
		return powf(aValue, aExp);
	}
	float Square(float aValue)
	{
		return aValue * aValue;
	}
	float Log(float aValue)
	{
		return logf(aValue);
	}
	float Log2(float aValue)
	{
		return log2f(aValue);
	}
	float Abs(float aValue)
	{
		return fabsf(aValue);
	}
	float Floor(float aValue)
	{
		return floorf(aValue);
	}
	float Ceil(float aValue)
	{
		return ceilf(aValue);
	}
	float Frac(float aValue)
	{
		return modf(aValue, nullptr);
	}
}