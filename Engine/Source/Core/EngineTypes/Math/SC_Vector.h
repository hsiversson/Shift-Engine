#pragma once

template<class T>
class SC_TVector3
{
public:
	T x,y,z;

public:
	/* Zero vector (0,0,0) */
	inline static SC_TVector3<T> ZeroVector() {	return SC_TVector3<T>(0, 0, 0); }

	/* One vector (1,1,1) */
	inline static SC_TVector3<T> OneVector() { return SC_TVector3<T>(1, 1, 1); }

	/* Up vector (0,1,0) */
	inline static SC_TVector3<T> UpVector() { return SC_TVector3<T>(0, 1, 0); }

	/* Down vector (0,-1,0) */
	inline static SC_TVector3<T> DownVector() { return SC_TVector3<T>(0, -1, 0); }

	/* Forward vector (0,0,1) */
	inline static SC_TVector3<T> ForwardVector() { return SC_TVector3<T>(0, 0, 1); }

	/* Backward vector (0,0,-1) */
	inline static SC_TVector3<T> BackwardVector() { return SC_TVector3<T>(0, 0, -1); }

	/* Right vector (1,0,0) */
	inline static SC_TVector3<T> RightVector() { return SC_TVector3<T>(1, 0, 0); }

	/* Left vector (-1,0,0) */
	inline static SC_TVector3<T> LeftVector() { return SC_TVector3<T>(-1, 0, 0); }
public:
	SC_TVector3() : x(0), y(0), z(0) {}
	SC_TVector3(const T& aValue) : x(aValue), y(aValue), z(aValue) {}
	SC_TVector3(const T& aX, const T& aY, const T& aZ) : x(aX), y(aY), z(aZ) {}
	SC_TVector3(const SC_TVector2<T>& aVector2, const T& aZ = static_cast<T>(0)) : x(aVector2.x), y(aVector2.y), z(aZ) {}
	SC_TVector3(const SC_TVector4<T>& aVector4) : x(aVector4.x), y(aVector4.y), z(aVector4.z) {}

	~SC_TVector3() {}

	template<class T2>
	SC_TVector3<T>(const SC_TVector3<T2>& aOther) : x(static_cast<T>(aOther.x)), y(static_cast<T>(aOther.y)), z(static_cast<T>(aOther.z)) {}

	//Negate operator
	SC_TVector3 operator-() const;

	// Subtraction operators
	SC_TVector3& operator-=(const SC_TVector3& aVector);
	SC_TVector3& operator-=(const T& aValue);
	SC_TVector3 operator-(const SC_TVector3& aVector) const;
	SC_TVector3 operator-(const T& aValue) const;

	//Multiplication operators
	SC_TVector3& operator*=(const SC_TVector3& aVector);
	SC_TVector3& operator*=(const T& aValue);
	SC_TVector3 operator*(const SC_TVector3& aVector) const;
	SC_TVector3 operator*(const T& aValue) const;

	//Addition operators
	SC_TVector3& operator+=(const SC_TVector3& aVector);
	SC_TVector3& operator+=(const T& aValue);
	SC_TVector3 operator+(const SC_TVector3& aVector) const;
	SC_TVector3 operator+(const T& aValue) const;

	//Division operators
	SC_TVector3& operator/=(const SC_TVector3& aVector);
	SC_TVector3& operator/=(const T& aValue);
	SC_TVector3 operator/(const SC_TVector3& aVector) const;
	SC_TVector3 operator/(const T& aValue) const;

	uint32 operator<(const SC_TVector3& aVector) const;
	uint32 operator<=(const SC_TVector3& aVector) const;
	uint32 operator>(const SC_TVector3& aVector) const;
	uint32 operator>=(const SC_TVector3& aVector) const;

	bool operator==(const SC_TVector3& aVector) const;
	bool operator!=(const SC_TVector3& aVector) const;

	T Dot(const SC_TVector3& aOther) const;
	SC_TVector3 Cross(const SC_TVector3& aOther)  const;

	T Length() const;
	T Length2() const;
	void Normalize();
	SC_TVector3 GetNormalized() const;

	T& operator[](uint32 aIndex);
	const T& operator[](uint32 aIndex) const;

	SC_TVector2<T> XY() const;
};

template<class T>
inline SC_TVector3<T> SC_TVector3<T>::operator-() const
{
	return SC_TVector3<T>(-x, -y, -z);
}

template<class T>
inline SC_TVector3<T>& SC_TVector3<T>::operator-=(const SC_TVector3<T>& aVector)
{
	x -= aVector.x;
	y -= aVector.y;
	z -= aVector.z;
	return *this;
}

template<class T>
inline SC_TVector3<T>& SC_TVector3<T>::operator-=(const T& aValue)
{
	x -= aValue;
	y -= aValue;
	z -= aValue;
	return *this;
}

template<class T>
inline SC_TVector3<T> SC_TVector3<T>::operator-(const SC_TVector3<T>& aVector) const
{
	return SC_TVector3<T>(x - aVector.x, y - aVector.y, z - aVector.z);
}

template<class T>
inline SC_TVector3<T> SC_TVector3<T>::operator-(const T& aValue) const
{
	return SC_TVector3<T>(x - aValue, y - aValue, z - aValue);
}

template<class T>
inline SC_TVector3<T>& SC_TVector3<T>::operator*=(const SC_TVector3<T>& aVector)
{
	x *= aVector.x;
	y *= aVector.y;
	z *= aVector.z;
	return *this;
}

template<class T>
inline SC_TVector3<T>& SC_TVector3<T>::operator*=(const T& aValue)
{
	x *= aValue;
	y *= aValue;
	z *= aValue;
	return *this;
}

template<class T>
inline SC_TVector3<T> SC_TVector3<T>::operator*(const SC_TVector3<T>& aVector) const
{
	return SC_TVector3<T>(x * aVector.x, y * aVector.y, z * aVector.z);
}

template<class T>
inline SC_TVector3<T> SC_TVector3<T>::operator*(const T& aValue) const
{
	return SC_TVector3<T>(x * aValue, y * aValue, z * aValue);
}

template<class T>
inline SC_TVector3<T>& SC_TVector3<T>::operator+=(const SC_TVector3<T>& aVector)
{
	x += aVector.x;
	y += aVector.y;
	z += aVector.z;
	return *this;
}

template<class T>
inline SC_TVector3<T>& SC_TVector3<T>::operator+=(const T& aValue)
{
	x += aValue;
	y += aValue;
	z += aValue;
	return *this;
}

template<class T>
inline SC_TVector3<T> SC_TVector3<T>::operator+(const SC_TVector3<T>& aVector) const
{
	return SC_TVector3<T>(x + aVector.x, y + aVector.y, z + aVector.z);
}

template<class T>
inline SC_TVector3<T> SC_TVector3<T>::operator+(const T& aValue) const
{
	return SC_TVector3<T>(x + aValue, y + aValue, z + aValue);
}

template<class T>
inline SC_TVector3<T>& SC_TVector3<T>::operator/=(const SC_TVector3<T>& aVector)
{
	x /= aVector.x;
	y /= aVector.y;
	z /= aVector.z;
	return *this;
}

template<class T>
inline SC_TVector3<T>& SC_TVector3<T>::operator/=(const T& aValue)
{
	x /= aValue;
	y /= aValue;
	z /= aValue;
	return *this;
}

template<class T>
inline SC_TVector3<T> SC_TVector3<T>::operator/(const SC_TVector3<T>& aVector) const
{
	return SC_TVector3<T>(x / aVector.x, y / aVector.y, z / aVector.z);
}

template<class T>
inline SC_TVector3<T> SC_TVector3<T>::operator/(const T& aValue) const
{
	return SC_TVector3<T>(x / aValue, y / aValue, z / aValue);
}

template<class T>
inline uint32 SC_TVector3<T>::operator<(const SC_TVector3<T>& aVector) const
{
	return (z < aVector.z ? 0x4 : 0) | (y < aVector.y ? 0x2 : 0) | (x < aVector.x ? 0x1 : 0);
}

template<class T>
inline uint32 SC_TVector3<T>::operator<=(const SC_TVector3<T>& aVector) const
{
	return (z <= aVector.z ? 0x4 : 0) | (y <= aVector.y ? 0x2 : 0) | (x <= aVector.x ? 0x1 : 0);
}

template<class T>
inline uint32 SC_TVector3<T>::operator>(const SC_TVector3<T>& aVector) const
{
	return (z > aVector.z ? 0x4 : 0) | (y > aVector.y ? 0x2 : 0) | (x > aVector.x ? 0x1 : 0);
}

template<class T>
inline uint32 SC_TVector3<T>::operator>=(const SC_TVector3<T>& aVector) const
{
	return (z >= aVector.z ? 0x4 : 0) | (y >= aVector.y ? 0x2 : 0) | (x >= aVector.x ? 0x1 : 0);
}

template<class T>
inline bool SC_TVector3<T>::operator==(const SC_TVector3<T>& aVector) const
{
	return (x == aVector.x) && (y == aVector.y) && (z == aVector.z);
}

template<class T>
inline bool SC_TVector3<T>::operator!=(const SC_TVector3<T>& aVector) const
{
	return !(*this == aVector);
}

template<class T>
inline T SC_TVector3<T>::Dot(const SC_TVector3<T>& aOther) const
{
	return x * aOther.x + y * aOther.y + z * aOther.z;
}

template<class T>
inline SC_TVector3<T> SC_TVector3<T>::Cross(const SC_TVector3<T>& aOther) const
{
	return SC_TVector3<T>(
		(y * aOther.z - z * aOther.y),
		(z * aOther.x - x * aOther.z),
		(x * aOther.y - y * aOther.x));
}

template<class T>
inline T SC_TVector3<T>::Length() const
{
	return SC_Math::Sqrt(x * x + y * y + z * z);
}

template<class T>
inline T SC_TVector3<T>::Length2() const
{
	return x * x + y * y + z * z;
}

template<class T>
inline void SC_TVector3<T>::Normalize()
{
	const float l2 = x * x + y * y + z * z;
	if (l2 > 0.f)
	{
		const float scale = SC_Math::InvSqrt(l2);
		x *= scale;
		y *= scale;
		z *= scale;
	}
}

template<class T>
inline SC_TVector3<T> SC_TVector3<T>::GetNormalized() const
{
	const float l2 = x * x + y * y + z * z;
	if (l2 > 0.f)
	{
		const float scale = SC_Math::InvSqrt(l2);
		return SC_TVector3<T>(x * scale, y * scale, z * scale);
	}
	return SC_TVector3<T>(0);
}

template<class T>
inline T& SC_TVector3<T>::operator[](uint32 aIndex)
{
	SC_ASSERT(aIndex >= 0 && aIndex < 3);
	return (aIndex == 2) ? z : ((aIndex == 1) ? y : x);
}

template<class T>
inline const T& SC_TVector3<T>::operator[](uint32 aIndex) const
{
	SC_ASSERT(aIndex >= 0 && aIndex < 3);
	return (aIndex == 2) ? z : ((aIndex == 1) ? y : x);
}

template<class T>
inline SC_TVector2<T> SC_TVector3<T>::XY() const
{
	return SC_TVector2<T>(x, y);
}

using SC_Vector = SC_TVector3<float>;
using SC_Vectorf = SC_TVector3<float>;
using SC_Vector3f = SC_TVector3<float>;

using SC_IntVector = SC_TVector3<int>;
using SC_IntVector3 = SC_TVector3<int>;
using SC_Vector3u = SC_TVector3<unsigned int>;