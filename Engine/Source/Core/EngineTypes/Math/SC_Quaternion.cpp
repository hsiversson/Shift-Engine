#include "SC_Quaternion.h"

SC_Quaternion::SC_Quaternion()
{

}

SC_Quaternion::SC_Quaternion(float aX, float aY, float aZ, float aW)
	: mVector(aX, aY, aZ, aW)
{

}

SC_Quaternion::SC_Quaternion(const SC_Vector4& aVector)
	: mVector(aVector)
{

}

SC_Quaternion::SC_Quaternion(const SC_Matrix& aMatrix)
{
	FromMatrix(aMatrix);
}

SC_Quaternion::~SC_Quaternion()
{

}

SC_Quaternion SC_Quaternion::Identity()
{
	return SC_Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

bool SC_Quaternion::IsNormalized() const
{
	static constexpr float NormalizedThreshold = 0.01f;
	return (SC_Math::Abs(1.0f - mVector.Length2()) <= NormalizedThreshold);
}

void SC_Quaternion::Normalize()
{
	mVector.Normalize();
}

SC_Quaternion SC_Quaternion::GetNormalized() const
{
	return SC_Quaternion(mVector.GetNormalized());
}

SC_Matrix SC_Quaternion::AsMatrix() const
{
	SC_Quaternion q(*this);
	if (!q.IsNormalized())
		q.Normalize();

	SC_Vector4 xAxis(1.0f - 2.0f*q.y*q.y - 2.0f*q.z*q.z, 2.0f*q.x*q.y - 2.0f*q.z*q.w, 2.0f*q.x*q.z + 2.0f*q.y*q.w, 0.0f);
	SC_Vector4 yAxis(2.0f*q.x*q.y + 2.0f*q.z*q.w, 1.0f - 2.0f*q.x*q.x - 2.0f*q.z*q.z, 2.0f*q.y*q.z - 2.0f*q.x*q.w, 0.0f);
	SC_Vector4 zAxis(2.0f*q.x*q.z - 2.0f*q.y*q.w, 2.0f*q.y*q.z + 2.0f*q.x*q.w, 1.0f - 2.0f*q.x*q.x - 2.0f*q.y*q.y, 0.0f);
	SC_Vector4 translation(0.0f, 0.0f, 0.0f, 1.0f);

	return SC_Matrix(xAxis, yAxis, zAxis, translation);
}

void SC_Quaternion::FromMatrix(const SC_Matrix& aMatrix)
{
	SC_Vector xAxis(aMatrix.mVectorX.XYZ()); // 00 01 02
	SC_Vector yAxis(aMatrix.mVectorY.XYZ()); // 10 11 12
	SC_Vector zAxis(aMatrix.mVectorZ.XYZ()); // 20 21 22
	xAxis.Normalize();
	yAxis.Normalize();
	zAxis.Normalize();

	w = SC_Math::Sqrt(1.0f + xAxis.x + yAxis.y + zAxis.z) / 2.0f;

	float t = 1.0f / (4.0f * w);
	x = (zAxis.y - yAxis.z) * t;
	y = (xAxis.z - zAxis.x) * t;
	z = (yAxis.x - xAxis.y) * t;
}

SC_Vector SC_Quaternion::AsEulerAngles(bool aAsDegrees) const
{
	float yaw = 0.0f;									// y
	float roll = SC_Math::Asin(2.0f*x*y + 2.0f*z*w);	// z
	float pitch = 0.0f;									// x

	const float t = x*y + z*w;
	if (t == 0.5f)
		yaw = 2.0f * SC_Math::Atan2(x, w);
	else if (t == -0.5f)
		yaw = -2.0f * SC_Math::Atan2(x, w);
	else
	{
		yaw = SC_Math::Atan2(2.0f*y*w - 2.0f*x*z, 1.0f - 2.0f*y*y - 2.0f*z*z);
		pitch = SC_Math::Atan2(2.0f*x*w - 2.0f*y*z, 1.0f - 2.0f*x*x - 2.0f*z*z);
	}

	if (aAsDegrees)
		return SC_Vector(SC_Math::RadiansToDegrees(pitch), SC_Math::RadiansToDegrees(yaw), SC_Math::RadiansToDegrees(roll));
	else
		return SC_Vector(pitch, yaw, roll);
}

SC_Quaternion SC_Quaternion::FromEulerAngles(const SC_Vector& aAngles, bool aIsDegrees)
{
	float yaw = aAngles.y * 0.5f;
	float roll = aAngles.z * 0.5f;
	float pitch = aAngles.x * 0.5f;

	if (aIsDegrees)
	{
		yaw = SC_Math::DegreesToRadians(yaw);
		roll = SC_Math::DegreesToRadians(roll);
		pitch = SC_Math::DegreesToRadians(pitch);
	}

	float s1 = 0.0f;
	float c1 = 0.0f;
	SC_Math::SinCos(&s1, &c1, yaw);

	float s2 = 0.0f;
	float c2 = 0.0f;
	SC_Math::SinCos(&s2, &c2, roll);

	float s3 = 0.0f;
	float c3 = 0.0f;
	SC_Math::SinCos(&s3, &c3, pitch);

	SC_Quaternion q;
	q.x = s1 * s2 * c3 + c1 * c2 * s3;
	q.y = s1 * c2 * c3 + c1 * s2 * s3;
	q.z = c1 * s2 * c3 - s1 * c2 * s3;
	q.w = c1 * c2 * c3 - s1 * s2 * s3;
	return q;
}

SC_Quaternion SC_Quaternion::CreateRotation(const SC_Vector& aAxis, float aAngle)
{
	float sinAngle = 0.0f;
	float cosAngle = 0.0f;
	SC_Math::SinCos(&sinAngle, &cosAngle, aAngle * 0.5f);

	return SC_Quaternion(SC_Vector4(aAxis * sinAngle, cosAngle));
}

SC_Quaternion SC_Quaternion::operator*(const SC_Quaternion& aOther) const
{
	SC_Quaternion q;
	q.x = (w*aOther.x + x*aOther.w + y*aOther.z - z*aOther.y);
	q.y = (w*aOther.y - x*aOther.z + y*aOther.w + z*aOther.x);
	q.z = (w*aOther.z + x*aOther.y - y*aOther.x + z*aOther.w);
	q.w = (w*aOther.w - x*aOther.x - y*aOther.y - z*aOther.z);
	return q;
}

SC_Quaternion SC_Quaternion::operator*=(const SC_Quaternion& aOther)
{
	*this = (*this * aOther);
	return *this;
}
