#pragma once

template<class T>
class SC_TVector2
{
public:
	T x, y;

public:
	SC_TVector2() : x(0), y(0) {}
	SC_TVector2(const T& aValue) : x(aValue), y(aValue) {}
	SC_TVector2(const T& aX, const T& aY) : x(aX), y(aY) {}
	~SC_TVector2() {}

	template<class T2>
	SC_TVector2<T>(const SC_TVector2<T2>& aOther) : x(static_cast<T>(aOther.x)), y(static_cast<T>(aOther.y)) {}

	T Length() const;
	T Length2() const;
	void Normalize();
	SC_TVector2 GetNormalized() const;

	//Negate operator
	SC_TVector2 operator-() const;

	// Subtraction operators
	SC_TVector2& operator-=(const SC_TVector2& aVector);
	SC_TVector2& operator-=(const T& aValue);
	SC_TVector2 operator-(const SC_TVector2& aVector) const;
	SC_TVector2 operator-(const T& aValue) const;

	//Multiplication operators
	SC_TVector2& operator*=(const SC_TVector2& aVector);
	SC_TVector2& operator*=(const T& aValue);
	SC_TVector2 operator*(const SC_TVector2& aVector) const;
	SC_TVector2 operator*(const T& aValue) const;

	//Addition operators
	SC_TVector2& operator+=(const SC_TVector2& aVector);
	SC_TVector2& operator+=(const T& aValue);
	SC_TVector2 operator+(const SC_TVector2& aVector) const;
	SC_TVector2 operator+(const T& aValue) const;

	//Division operators
	SC_TVector2& operator/=(const SC_TVector2& aVector);
	SC_TVector2& operator/=(const T& aValue);
	SC_TVector2 operator/(const SC_TVector2& aVector) const;
	SC_TVector2 operator/(const T& aValue) const;

	uint32 operator<(const SC_TVector2& aVector) const;
	uint32 operator<=(const SC_TVector2& aVector) const;
	uint32 operator>(const SC_TVector2& aVector) const;
	uint32 operator>=(const SC_TVector2& aVector) const;

	bool operator==(const SC_TVector2& aVector) const;
	bool operator!=(const SC_TVector2& aVector) const;
};

template<class T>
inline T SC_TVector2<T>::Length() const
{
	return SC_Math::Sqrt(x * x + y * y);
}

template<class T>
inline T SC_TVector2<T>::Length2() const
{
	return (x * x + y * y);
}

template<class T>
inline void SC_TVector2<T>::Normalize()
{
	const float l2 = x * x + y * y;
	if (l2 > 0.f)
	{
		const float scale = SC_Math::InvSqrt(l2);
		x *= scale;
		y *= scale;
	}
}

template<class T>
inline SC_TVector2<T> SC_TVector2<T>::GetNormalized() const
{
	const float l2 = x * x + y * y;
	if (l2 > 0.f)
	{
		const float scale = SC_Math::InvSqrt(l2);
		return SC_TVector2<T>(x * scale, y * scale);
	}
	return SC_TVector2<T>(0);
}

template<class T>
inline SC_TVector2<T> SC_TVector2<T>::operator-() const
{
	return SC_TVector2<T>(-x, -y);
}

template<class T>
inline SC_TVector2<T>& SC_TVector2<T>::operator-=(const SC_TVector2<T>& aVector)
{
	x -= aVector.x;
	y -= aVector.y;
	return *this;
}

template<class T>
inline SC_TVector2<T>& SC_TVector2<T>::operator-=(const T& aValue)
{
	x -= aValue;
	y -= aValue;
	return *this;
}

template<class T>
inline SC_TVector2<T> SC_TVector2<T>::operator-(const SC_TVector2<T>& aVector) const
{
	return SC_TVector2<T>(x - aVector.x, y - aVector.y);
}

template<class T>
inline SC_TVector2<T> SC_TVector2<T>::operator-(const T& aValue) const
{
	return SC_TVector2<T>(x - aValue, y - aValue);
}

template<class T>
inline SC_TVector2<T>& SC_TVector2<T>::operator*=(const SC_TVector2<T>& aVector)
{
	x *= aVector.x;
	y *= aVector.y;
	return *this;
}

template<class T>
inline SC_TVector2<T>& SC_TVector2<T>::operator*=(const T& aValue)
{
	x *= aValue;
	y *= aValue;
	return *this;
}

template<class T>
inline SC_TVector2<T> SC_TVector2<T>::operator*(const SC_TVector2<T>& aVector) const
{
	return SC_TVector2<T>(x * aVector.x, y * aVector.y);
}

template<class T>
inline SC_TVector2<T> SC_TVector2<T>::operator*(const T& aValue) const
{
	return SC_TVector2<T>(x * aValue, y * aValue);
}

template<class T>
inline SC_TVector2<T>& SC_TVector2<T>::operator+=(const SC_TVector2<T>& aVector)
{
	x += aVector.x;
	y += aVector.y;
	return *this;
}

template<class T>
inline SC_TVector2<T>& SC_TVector2<T>::operator+=(const T& aValue)
{
	x += aValue;
	y += aValue;
	return *this;
}

template<class T>
inline SC_TVector2<T> SC_TVector2<T>::operator+(const SC_TVector2<T>& aVector) const
{
	return SC_TVector2<T>(x + aVector.x, y + aVector.y);
}

template<class T>
inline SC_TVector2<T> SC_TVector2<T>::operator+(const T& aValue) const
{
	return SC_TVector2<T>(x + aValue, y + aValue);
}

template<class T>
inline SC_TVector2<T>& SC_TVector2<T>::operator/=(const SC_TVector2<T>& aVector)
{
	x /= aVector.x;
	y /= aVector.y;
	return *this;
}

template<class T>
inline SC_TVector2<T>& SC_TVector2<T>::operator/=(const T& aValue)
{
	x /= aValue;
	y /= aValue;
	return *this;
}

template<class T>
inline SC_TVector2<T> SC_TVector2<T>::operator/(const SC_TVector2<T>& aVector) const
{
	return SC_TVector2<T>(x / aVector.x, y / aVector.y);
}

template<class T>
inline SC_TVector2<T> SC_TVector2<T>::operator/(const T& aValue) const
{
	return SC_TVector2<T>(x / aValue, y / aValue);
}

template<class T>
inline uint32 SC_TVector2<T>::operator<(const SC_TVector2& aVector) const
{
	return (y < aVector.y ? 0x2 : 0) | (x < aVector.x ? 0x1 : 0);
}

template<class T>
inline uint32 SC_TVector2<T>::operator<=(const SC_TVector2& aVector) const
{
	return (y <= aVector.y ? 0x2 : 0) | (x <= aVector.x ? 0x1 : 0);
}

template<class T>
inline uint32 SC_TVector2<T>::operator>(const SC_TVector2& aVector) const
{
	return (y > aVector.y ? 0x2 : 0) | (x > aVector.x ? 0x1 : 0);
}

template<class T>
inline uint32 SC_TVector2<T>::operator>=(const SC_TVector2& aVector) const
{
	return (y >= aVector.y ? 0x2 : 0) | (x >= aVector.x ? 0x1 : 0);
}


template<class T>
bool SC_TVector2<T>::operator==(const SC_TVector2& aVector) const
{
	return (x == aVector.x) && (y == aVector.y);
}

template<class T>
bool SC_TVector2<T>::operator!=(const SC_TVector2& aVector) const
{
	return !(*this == aVector);
}

using SC_Vector2 = SC_TVector2<float>;
using SC_Vector2f = SC_TVector2<float>;
using SC_IntVector2 = SC_TVector2<int>;