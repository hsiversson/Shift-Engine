#pragma once
#include "Debugging/SC_Assert.h"
#include "SIMD/SC_VectorInstructions.h"

template<class T>
class alignas(16) SC_TVector4
{
public:
	union
	{
		struct
		{
			T x, y, z, w;
		};
		SC_VectorInstructions::VectorRegister mRegister;
	};

public:
	SC_TVector4() : x(0), y(0), z(0), w(0) {}
	SC_TVector4(const T& aValue) : x(aValue), y(aValue), z(aValue), w(aValue) {}
	SC_TVector4(const SC_TVector2<T>& aVector0, const SC_TVector2<T>& aVector1) : x(aVector0.x), y(aVector0.y), z(aVector1.x), w(aVector1.y) {}
	SC_TVector4(const SC_TVector3<T>& aVector3, float aW = 1.0f) : x(aVector3.x), y(aVector3.y), z(aVector3.z), w(aW) {}
	SC_TVector4(const T& aX, const T& aY, const T& aZ, const T& aW) : x(aX), y(aY), z(aZ), w(aW) {}

	template<class T2>
	SC_TVector4<T>(const SC_TVector4<T2>& aOther) : x(static_cast<T>(aOther.x)), y(static_cast<T>(aOther.y)), z(static_cast<T>(aOther.z)), w(static_cast<T>(aOther.w)) {}

	void Normalize();
	SC_TVector4 GetNormalized() const;

	T Length() const;
	T Length2() const;

	SC_TVector2<T> XY() const { return SC_TVector2<T>(x, y); }
	SC_TVector2<T> XZ() const { return SC_TVector2<T>(x, z); }
	SC_TVector2<T> YZ() const { return SC_TVector2<T>(y, z); }

	SC_TVector2<T> ZW() const { return SC_TVector2<T>(z, w); }
	SC_TVector3<T> XYZ() const { return SC_TVector3<T>(x, y, z); }

	//Negate operator
	SC_TVector4 operator-() const;

	// Subtraction operators
	SC_TVector4& operator-=(const SC_TVector4& aVector);
	SC_TVector4& operator-=(const T& aValue);
	SC_TVector4 operator-(const SC_TVector4& aVector) const;
	SC_TVector4 operator-(const T& aValue) const;

	//Multiplication operators
	SC_TVector4& operator*=(const SC_TVector4& aVector);
	SC_TVector4& operator*=(const T& aValue);
	SC_TVector4 operator*(const SC_TVector4& aVector) const;
	SC_TVector4 operator*(const T& aValue) const;

	//Addition operators
	SC_TVector4& operator+=(const SC_TVector4& aVector);
	SC_TVector4& operator+=(const T& aValue);
	SC_TVector4 operator+(const SC_TVector4& aVector) const;
	SC_TVector4 operator+(const T& aValue) const;

	//Division operators
	SC_TVector4& operator/=(const SC_TVector4& aVector);
	SC_TVector4& operator/=(const T& aValue);
	SC_TVector4 operator/(const SC_TVector4& aVector) const;
	SC_TVector4 operator/(const T& aValue) const;

	// Comparison operators
	bool operator==(const SC_TVector4<T>& aVector) const;
	bool operator!=(const SC_TVector4<T>& aVector) const;

	T& operator[](const uint32 aIndex);
	const T& operator[](const uint32 aIndex) const;
};

template<class T>
inline void SC_TVector4<T>::Normalize()
{
	const float l2 = x * x + y * y + z * z + w * w;
	if (l2 > 0.f)
	{
		const float scale = SC_Math::InvSqrt(l2);
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
	}
}

template<class T>
inline SC_TVector4<T> SC_TVector4<T>::GetNormalized() const
{
	SC_TVector4<T> v(*this);
	v.Normalize();
	return v;
}

template<class T>
inline T SC_TVector4<T>::Length() const
{
	return SC_Math::Sqrt(x*x + y*y + z*z + w*w);
}

template<class T>
inline T SC_TVector4<T>::Length2() const
{
	return x*x + y*y + z*z + w*w;
}

template<class T>
inline SC_TVector4<T> SC_TVector4<T>::operator-() const
{
	return SC_TVector4<T>(-x, -y, -z, -w);
}

template<class T>
inline SC_TVector4<T>& SC_TVector4<T>::operator-=(const SC_TVector4<T>& aVector)
{
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Subtract(mRegister, aVector.mRegister), x, y, z, w);
	return *this;
}

template<class T>
inline SC_TVector4<T>& SC_TVector4<T>::operator-=(const T& aValue)
{
	SC_VectorInstructions::VectorRegister reg1 = SC_VectorInstructions::Load(aValue);
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Subtract(mRegister, reg1), x, y, z, w);
	return *this;
}

template<class T>
inline SC_TVector4<T> SC_TVector4<T>::operator-(const SC_TVector4<T>& aVector) const
{
	SC_TVector4<T> result;
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Subtract(mRegister, aVector.mRegister), result.x, result.y, result.z, result.w);
	return result;
}

template<class T>
inline SC_TVector4<T> SC_TVector4<T>::operator-(const T& aValue) const
{
	SC_VectorInstructions::VectorRegister reg1 = SC_VectorInstructions::Load(aValue);

	SC_TVector4<T> result;
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Subtract(mRegister, reg1), result.x, result.y, result.z, result.w);
	return result;
}

template<class T>
inline SC_TVector4<T>& SC_TVector4<T>::operator*=(const SC_TVector4<T>& aVector)
{
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Multiply(mRegister, aVector.mRegister), x, y, z, w);
	return *this;
}

template<class T>
inline SC_TVector4<T>& SC_TVector4<T>::operator*=(const T& aValue)
{
	SC_VectorInstructions::VectorRegister reg1 = SC_VectorInstructions::Load(aValue);
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Multiply(mRegister, reg1), x, y, z, w);
	return *this;
}

template<class T>
inline SC_TVector4<T> SC_TVector4<T>::operator*(const SC_TVector4<T>& aVector) const
{
	SC_TVector4<T> result;
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Multiply(mRegister, aVector.mRegister), result.x, result.y, result.z, result.w);
	return result;
}

template<class T>
inline SC_TVector4<T> SC_TVector4<T>::operator*(const T& aValue) const
{
	SC_VectorInstructions::VectorRegister reg1 = SC_VectorInstructions::Load(aValue);

	SC_TVector4<T> result;
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Multiply(mRegister, reg1), result.x, result.y, result.z, result.w);
	return result;
}

template<class T>
inline SC_TVector4<T>& SC_TVector4<T>::operator+=(const SC_TVector4<T>& aVector)
{
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Add(mRegister, aVector.mRegister), x, y, z, w);
	return *this;
}

template<class T>
inline SC_TVector4<T>& SC_TVector4<T>::operator+=(const T& aValue)
{
	SC_VectorInstructions::VectorRegister reg1 = SC_VectorInstructions::Load(aValue);
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Add(mRegister, reg1), x, y, z, w);
	return *this;
}

template<class T>
inline SC_TVector4<T> SC_TVector4<T>::operator+(const SC_TVector4<T>& aVector) const
{
	SC_TVector4<T> result;
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Add(mRegister, aVector.mRegister), result.x, result.y, result.z, result.w);
	return result;
}

template<class T>
inline SC_TVector4<T> SC_TVector4<T>::operator+(const T& aValue) const
{
	SC_VectorInstructions::VectorRegister reg1 = SC_VectorInstructions::Load(aValue);

	SC_TVector4<T> result;
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Add(mRegister, reg1), result.x, result.y, result.z, result.w);
	return result;
}

template<class T>
inline SC_TVector4<T>& SC_TVector4<T>::operator/=(const SC_TVector4<T>& aVector)
{
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Divide(mRegister, aVector.mRegister), x, y, z, w);
	return *this;
}

template<class T>
inline SC_TVector4<T>& SC_TVector4<T>::operator/=(const T& aValue)
{
	SC_VectorInstructions::VectorRegister reg1 = SC_VectorInstructions::Load(aValue);
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Divide(mRegister, reg1), x, y, z, w);
	return *this;
}

template<class T>
inline SC_TVector4<T> SC_TVector4<T>::operator/(const SC_TVector4<T>& aVector) const
{
	SC_TVector4<T> result;
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Divide(mRegister, aVector.mRegister), result.x, result.y, result.z, result.w);
	return result;
}

template<class T>
inline SC_TVector4<T> SC_TVector4<T>::operator/(const T& aValue) const
{
	SC_VectorInstructions::VectorRegister reg1 = SC_VectorInstructions::Load(aValue);

	SC_TVector4<T> result;
	SC_VectorInstructions::GetVector(SC_VectorInstructions::Divide(mRegister, reg1), result.x, result.y, result.z, result.w);
	return result;
}

template<class T>
bool SC_TVector4<T>::operator==(const SC_TVector4<T>& aVector) const
{
	return (x == aVector.x && y == aVector.y && z == aVector.z && w == aVector.w);
}

template<class T>
T& SC_TVector4<T>::operator[](const uint32 aIndex)
{
	assert(aIndex < 4);
	return (aIndex == 0) ? x : ((aIndex == 1) ? y : ((aIndex == 2) ? z : w));
}

template<class T>
const T& SC_TVector4<T>::operator[](const uint32 aIndex) const
{
	assert(aIndex < 4);
	return (aIndex == 0) ? x : ((aIndex == 1) ? y : ((aIndex == 2) ? z : w));
}

template<class T>
bool SC_TVector4<T>::operator!=(const SC_TVector4<T>& aVector) const
{
	return !(*this == aVector);
}

using SC_Vector4 = SC_TVector4<float>;
using SC_Vector4f = SC_TVector4<float>;
using SC_IntVector4 = SC_TVector4<int>;