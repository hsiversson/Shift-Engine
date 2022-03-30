#pragma once
#include "SC_Vector4.h"
#include "Platform/Misc/SC_MemoryFunctions.h"

class alignas(16) SC_Matrix
{
public:
	union
	{
		alignas(16) float m[16];
		alignas(16) float m44[4][4];

		struct
		{
			SC_Vector4 mVectorX;
			SC_Vector4 mVectorY;
			SC_Vector4 mVectorZ;
			SC_Vector4 mVectorW;
		};
	};

public:
	SC_Matrix();
	SC_Matrix(const SC_Matrix& aMatrix);
	SC_Matrix(const SC_Vector4& aXAxis, const SC_Vector4& aYAxis, const SC_Vector4& aZAxis, const SC_Vector4& aTranslation);
	SC_Matrix(float M00, float M01, float M02, float M03,
		float M10, float M11, float M12, float M13,
		float M20, float M21, float M22, float M23,
		float M30, float M31, float M32, float M33);
	~SC_Matrix();

	SC_Matrix operator*(const SC_Matrix& aMatrix) const;
	SC_Matrix& operator*=(const SC_Matrix& aMatrix);

	SC_Matrix FastInverse() const;
	SC_Matrix Inverse() const;

	void SetTranslation(const SC_Vector4& aTranslation);
	SC_Vector4 GetTranslation() const;
	void SetPosition(const SC_Vector& aPosition);
	SC_Vector GetPosition() const;

	SC_Vector GetRight() const;
	SC_Vector GetUp() const;
	SC_Vector GetForward() const;

	void Rotate(const SC_Vector& aAxis, float aRotationAmount);

	float& operator[](uint32 aIndex);
	const float& operator[](uint32 aIndex) const;

	static SC_Matrix Identity();
	static SC_Matrix CreateRotation(const SC_Vector& aAxis, float aAngle);

protected:
	void InverseInternal(void* aDstMatrix, const void* aSrcMatrix) const;
	float Determinant() const;
};

inline SC_Matrix::SC_Matrix()
	: mVectorX(SC_Vector4())
	, mVectorY(SC_Vector4())
	, mVectorZ(SC_Vector4())
	, mVectorW(SC_Vector4())
{

}

inline SC_Matrix::SC_Matrix(const SC_Matrix& aMatrix)
{
	m[0] = aMatrix.m[0]; m[1] = aMatrix.m[1]; m[2] = aMatrix.m[2]; m[3] = aMatrix.m[3];
	m[4] = aMatrix.m[4]; m[5] = aMatrix.m[5]; m[6] = aMatrix.m[6]; m[7] = aMatrix.m[7];
	m[8] = aMatrix.m[8]; m[9] = aMatrix.m[9]; m[10] = aMatrix.m[10]; m[11] = aMatrix.m[11];
	m[12] = aMatrix.m[12]; m[13] = aMatrix.m[13]; m[14] = aMatrix.m[14]; m[15] = aMatrix.m[15];
}

inline SC_Matrix::SC_Matrix(const SC_Vector4& aXAxis, const SC_Vector4& aYAxis, const SC_Vector4& aZAxis, const SC_Vector4& aTranslation)
{
	m[0] = aXAxis.x;		 m[1] = aXAxis.y;		  m[2] = aXAxis.z;	   m[3] = aXAxis.w;
	m[4] = aYAxis.x;		 m[5] = aYAxis.y;		  m[6] = aYAxis.z;	   m[7] = aYAxis.w;
	m[8] = aZAxis.x;		 m[9] = aZAxis.y;		  m[10] = aZAxis.z;	   m[11] = aZAxis.w;
	m[12] = aTranslation.x; m[13] = aTranslation.y; m[14] = aTranslation.z; m[15] = aTranslation.w;
}

inline SC_Matrix::SC_Matrix(float M00, float M01, float M02, float M03, float M10, float M11, float M12, float M13, float M20, float M21, float M22, float M23, float M30, float M31, float M32, float M33)
{
	m[0] = M00; m[1] = M01; m[2] = M02; m[3] = M03;
	m[4] = M10; m[5] = M11; m[6] = M12; m[7] = M13;
	m[8] = M20; m[9] = M21; m[10] = M22; m[11] = M23;
	m[12] = M30; m[13] = M31; m[14] = M32; m[15] = M33;
}

inline SC_Matrix::~SC_Matrix()
{

}

inline SC_Matrix SC_Matrix::operator*(const SC_Matrix& aMatrix) const
{
	SC_Matrix outMatrix;
	const SC_Matrix& _this = *this;
	outMatrix[0] = _this[0] * aMatrix[0] + _this[1] * aMatrix[4] + _this[2] * aMatrix[8] + _this[3] * aMatrix[12];
	outMatrix[1] = _this[0] * aMatrix[1] + _this[1] * aMatrix[5] + _this[2] * aMatrix[9] + _this[3] * aMatrix[13];
	outMatrix[2] = _this[0] * aMatrix[2] + _this[1] * aMatrix[6] + _this[2] * aMatrix[10] + _this[3] * aMatrix[14];
	outMatrix[3] = _this[0] * aMatrix[3] + _this[1] * aMatrix[7] + _this[2] * aMatrix[11] + _this[3] * aMatrix[15];

	outMatrix[4] = _this[4] * aMatrix[0] + _this[5] * aMatrix[4] + _this[6] * aMatrix[8] + _this[7] * aMatrix[12];
	outMatrix[5] = _this[4] * aMatrix[1] + _this[5] * aMatrix[5] + _this[6] * aMatrix[9] + _this[7] * aMatrix[13];
	outMatrix[6] = _this[4] * aMatrix[2] + _this[5] * aMatrix[6] + _this[6] * aMatrix[10] + _this[7] * aMatrix[14];
	outMatrix[7] = _this[4] * aMatrix[3] + _this[5] * aMatrix[7] + _this[6] * aMatrix[11] + _this[7] * aMatrix[15];

	outMatrix[8] = _this[8] * aMatrix[0] + _this[9] * aMatrix[4] + _this[10] * aMatrix[8] + _this[11] * aMatrix[12];
	outMatrix[9] = _this[8] * aMatrix[1] + _this[9] * aMatrix[5] + _this[10] * aMatrix[9] + _this[11] * aMatrix[13];
	outMatrix[10] = _this[8] * aMatrix[2] + _this[9] * aMatrix[6] + _this[10] * aMatrix[10] + _this[11] * aMatrix[14];
	outMatrix[11] = _this[8] * aMatrix[3] + _this[9] * aMatrix[7] + _this[10] * aMatrix[11] + _this[11] * aMatrix[15];

	outMatrix[12] = _this[12] * aMatrix[0] + _this[13] * aMatrix[4] + _this[14] * aMatrix[8] + _this[15] * aMatrix[12];
	outMatrix[13] = _this[12] * aMatrix[1] + _this[13] * aMatrix[5] + _this[14] * aMatrix[9] + _this[15] * aMatrix[13];
	outMatrix[14] = _this[12] * aMatrix[2] + _this[13] * aMatrix[6] + _this[14] * aMatrix[10] + _this[15] * aMatrix[14];
	outMatrix[15] = _this[12] * aMatrix[3] + _this[13] * aMatrix[7] + _this[14] * aMatrix[11] + _this[15] * aMatrix[15];
	return outMatrix;
}

inline SC_Matrix& SC_Matrix::operator*=(const SC_Matrix& aMatrix)
{
	*this = *this * aMatrix;
	return *this;
}

inline SC_Matrix SC_Matrix::FastInverse() const
{
	SC_Matrix result;
	//float scaleX = mVectorX.Length2_3D();
	//float scaleY = myAxisY.Length2_3D();
	//float scaleZ = myAxisZ.Length2_3D();
	//bool result = (scaleX > 0.000001f) && (scaleY > 0.000001f) && (scaleZ > 0.000001f) && (myPosition.w == 1);
	//
	//if (result)
	//{
	//	scaleX = 1.f / scaleX;
	//	scaleY = 1.f / scaleY;
	//	scaleZ = 1.f / scaleZ;
	//
	//	result = SC_Matrix(myAxisX.x * scaleX, myAxisY.x * scaleY, myAxisZ.x * scaleZ, 0.f,
	//		myAxisX.y * scaleX, myAxisY.y * scaleY, myAxisZ.y * scaleZ, 0.f,
	//		myAxisX.z * scaleX, myAxisY.z * scaleY, myAxisZ.z * scaleZ, 0.f,
	//		-(myPosition.x * myAxisX.x + myPosition.y * myAxisX.y + myPosition.z * myAxisX.z) * scaleX,
	//		-(myPosition.x * myAxisY.x + myPosition.y * myAxisY.y + myPosition.z * myAxisY.z) * scaleY,
	//		-(myPosition.x * myAxisZ.x + myPosition.y * myAxisZ.y + myPosition.z * myAxisZ.z) * scaleZ,
	//		1.f);
	//}

	return result;
}

inline SC_Matrix SC_Matrix::Inverse() const
{
	SC_Matrix result;

	SC_Vector xAxis(m[0], m[1], m[2]);
	SC_Vector yAxis(m[4], m[5], m[6]);
	SC_Vector zAxis(m[8], m[9], m[10]);

	const float	det = Determinant();

	if (det == 0.0f)
	{
		result = SC_Matrix::Identity();
	}
	else
	{
#if ENABLE_SSEINTRINSICS
		SVectorInstructions::MatrixInverse(&result, this);
#else
		InverseInternal(&result, this);
#endif
	}

	return result;
}

inline void SC_Matrix::SetTranslation(const SC_Vector4& aTranslation)
{
	m[12] = aTranslation.x;
	m[13] = aTranslation.y;
	m[14] = aTranslation.z;
	m[15] = aTranslation.w;
}

inline SC_Vector4 SC_Matrix::GetTranslation() const
{
	return SC_Vector4(m[12], m[13], m[14], m[15]);
}

inline void SC_Matrix::SetPosition(const SC_Vector& aPosition)
{
	m[12] = aPosition.x;
	m[13] = aPosition.y;
	m[14] = aPosition.z;
}

inline SC_Vector SC_Matrix::GetPosition() const
{
	return SC_Vector(m[12], m[13], m[14]);
}

inline SC_Vector SC_Matrix::GetRight() const
{
	SC_Vector right = SC_Vector(m[0], m[1], m[2]);
	return right.GetNormalized();
}

inline SC_Vector SC_Matrix::GetUp() const
{
	SC_Vector up = SC_Vector(m[4], m[5], m[6]);
	return up.GetNormalized();
}

inline SC_Vector SC_Matrix::GetForward() const
{
	SC_Vector forward = SC_Vector(m[8], m[9], m[10]);
	return forward.GetNormalized();
}

inline void SC_Matrix::Rotate(const SC_Vector& aAxis, float aRotationAmount)
{
	SC_Matrix& _this = *this;
	const SC_Vector pos = GetPosition();
	_this = _this * CreateRotation(aAxis, aRotationAmount);
	SetTranslation(SC_Vector4(pos.x, pos.y, pos.z, 1.0f));
}

inline float& SC_Matrix::operator[](uint32 aIndex)
{
	return m[aIndex];
}

inline const float& SC_Matrix::operator[](uint32 aIndex) const
{
	return m[aIndex];
}

inline SC_Matrix SC_Matrix::Identity()
{
	return SC_Matrix(SC_Vector4(1, 0, 0, 0), SC_Vector4(0, 1, 0, 0), SC_Vector4(0, 0, 1, 0), SC_Vector4(0, 0, 0, 1));
}

inline SC_Matrix SC_Matrix::CreateRotation(const SC_Vector& aAxis, float aAngle)
{
	float sina, cosa;
	SC_Math::SinCos(&sina, &cosa, aAngle);

	const float omc = 1.0f - cosa;
	const float xx = aAxis.x * aAxis.x;
	const float yy = aAxis.y * aAxis.y;
	const float zz = aAxis.z * aAxis.z;
	const float xy = aAxis.x * aAxis.y;
	const float xz = aAxis.x * aAxis.z;
	const float yz = aAxis.y * aAxis.z;

	return SC_Matrix(
		xx + cosa * (1.0f - xx), xy * omc - aAxis.z * sina, xz * omc + aAxis.y * sina, 0.f,
		xy * omc + aAxis.z * sina, yy + cosa * (1.0f - yy), yz * omc - aAxis.x * sina, 0.f,
		xz * omc - aAxis.y * sina, yz * omc + aAxis.x * sina, zz + cosa * (1.0f - zz), 0.f,
		0.f, 0.f, 0.f, 1.f);
}


inline void SC_Matrix::InverseInternal(void* aDstMatrix, const void* aSrcMatrix) const
{
	SC_Matrix result;
	const SC_Matrix& src = *((const SC_Matrix*)aSrcMatrix);
	float det[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	SC_Matrix tmp;

	tmp.m44[0][0] = src.m44[2][2] * src.m44[3][3] - src.m44[2][3] * src.m44[3][2];
	tmp.m44[0][1] = src.m44[1][2] * src.m44[3][3] - src.m44[1][3] * src.m44[3][2];
	tmp.m44[0][2] = src.m44[1][2] * src.m44[2][3] - src.m44[1][3] * src.m44[2][2];

	tmp.m44[1][0] = src.m44[2][2] * src.m44[3][3] - src.m44[2][3] * src.m44[3][2];
	tmp.m44[1][1] = src.m44[0][2] * src.m44[3][3] - src.m44[0][3] * src.m44[3][2];
	tmp.m44[1][2] = src.m44[0][2] * src.m44[2][3] - src.m44[0][3] * src.m44[2][2];

	tmp.m44[2][0] = src.m44[1][2] * src.m44[3][3] - src.m44[1][3] * src.m44[3][2];
	tmp.m44[2][1] = src.m44[0][2] * src.m44[3][3] - src.m44[0][3] * src.m44[3][2];
	tmp.m44[2][2] = src.m44[0][2] * src.m44[1][3] - src.m44[0][3] * src.m44[1][2];

	tmp.m44[3][0] = src.m44[1][2] * src.m44[2][3] - src.m44[1][3] * src.m44[2][2];
	tmp.m44[3][1] = src.m44[0][2] * src.m44[2][3] - src.m44[0][3] * src.m44[2][2];
	tmp.m44[3][2] = src.m44[0][2] * src.m44[1][3] - src.m44[0][3] * src.m44[1][2];

	det[0] = src.m44[1][1] * tmp.m44[0][0] - src.m44[2][1] * tmp.m44[0][1] + src.m44[3][1] * tmp.m44[0][2];
	det[1] = src.m44[0][1] * tmp.m44[1][0] - src.m44[2][1] * tmp.m44[1][1] + src.m44[3][1] * tmp.m44[1][2];
	det[2] = src.m44[0][1] * tmp.m44[2][0] - src.m44[1][1] * tmp.m44[2][1] + src.m44[3][1] * tmp.m44[2][2];
	det[3] = src.m44[0][1] * tmp.m44[3][0] - src.m44[1][1] * tmp.m44[3][1] + src.m44[2][1] * tmp.m44[3][2];

	float determinant = src.m44[0][0] * det[0] - src.m44[1][0] * det[1] + src.m44[2][0] * det[2] - src.m44[3][0] * det[3];
	const float	rDet = 1.0f / determinant;

	result.m44[0][0] = rDet * det[0];
	result.m44[0][1] = -rDet * det[1];
	result.m44[0][2] = rDet * det[2];
	result.m44[0][3] = -rDet * det[3];
	result.m44[1][0] = -rDet * (src.m44[1][0] * tmp.m44[0][0] - src.m44[2][0] * tmp.m44[0][1] + src.m44[3][0] * tmp.m44[0][2]);
	result.m44[1][1] = rDet * (src.m44[0][0] * tmp.m44[1][0] - src.m44[2][0] * tmp.m44[1][1] + src.m44[3][0] * tmp.m44[1][2]);
	result.m44[1][2] = -rDet * (src.m44[0][0] * tmp.m44[2][0] - src.m44[1][0] * tmp.m44[2][1] + src.m44[3][0] * tmp.m44[2][2]);
	result.m44[1][3] = rDet * (src.m44[0][0] * tmp.m44[3][0] - src.m44[1][0] * tmp.m44[3][1] + src.m44[2][0] * tmp.m44[3][2]);
	result.m44[2][0] = rDet * (
		src.m44[1][0] * (src.m44[2][1] * src.m44[3][3] - src.m44[2][3] * src.m44[3][1]) -
		src.m44[2][0] * (src.m44[1][1] * src.m44[3][3] - src.m44[1][3] * src.m44[3][1]) +
		src.m44[3][0] * (src.m44[1][1] * src.m44[2][3] - src.m44[1][3] * src.m44[2][1])
		);
	result.m44[2][1] = -rDet * (
		src.m44[0][0] * (src.m44[2][1] * src.m44[3][3] - src.m44[2][3] * src.m44[3][1]) -
		src.m44[2][0] * (src.m44[0][1] * src.m44[3][3] - src.m44[0][3] * src.m44[3][1]) +
		src.m44[3][0] * (src.m44[0][1] * src.m44[2][3] - src.m44[0][3] * src.m44[2][1])
		);
	result.m44[2][2] = rDet * (
		src.m44[0][0] * (src.m44[1][1] * src.m44[3][3] - src.m44[1][3] * src.m44[3][1]) -
		src.m44[1][0] * (src.m44[0][1] * src.m44[3][3] - src.m44[0][3] * src.m44[3][1]) +
		src.m44[3][0] * (src.m44[0][1] * src.m44[1][3] - src.m44[0][3] * src.m44[1][1])
		);
	result.m44[2][3] = -rDet * (
		src.m44[0][0] * (src.m44[1][1] * src.m44[2][3] - src.m44[1][3] * src.m44[2][1]) -
		src.m44[1][0] * (src.m44[0][1] * src.m44[2][3] - src.m44[0][3] * src.m44[2][1]) +
		src.m44[2][0] * (src.m44[0][1] * src.m44[1][3] - src.m44[0][3] * src.m44[1][1])
		);
	result.m44[3][0] = -rDet * (
		src.m44[1][0] * (src.m44[2][1] * src.m44[3][2] - src.m44[2][2] * src.m44[3][1]) -
		src.m44[2][0] * (src.m44[1][1] * src.m44[3][2] - src.m44[1][2] * src.m44[3][1]) +
		src.m44[3][0] * (src.m44[1][1] * src.m44[2][2] - src.m44[1][2] * src.m44[2][1])
		);
	result.m44[3][1] = rDet * (
		src.m44[0][0] * (src.m44[2][1] * src.m44[3][2] - src.m44[2][2] * src.m44[3][1]) -
		src.m44[2][0] * (src.m44[0][1] * src.m44[3][2] - src.m44[0][2] * src.m44[3][1]) +
		src.m44[3][0] * (src.m44[0][1] * src.m44[2][2] - src.m44[0][2] * src.m44[2][1])
		);
	result.m44[3][2] = -rDet * (
		src.m44[0][0] * (src.m44[1][1] * src.m44[3][2] - src.m44[1][2] * src.m44[3][1]) -
		src.m44[1][0] * (src.m44[0][1] * src.m44[3][2] - src.m44[0][2] * src.m44[3][1]) +
		src.m44[3][0] * (src.m44[0][1] * src.m44[1][2] - src.m44[0][2] * src.m44[1][1])
		);
	result.m44[3][3] = rDet * (
		src.m44[0][0] * (src.m44[1][1] * src.m44[2][2] - src.m44[1][2] * src.m44[2][1]) -
		src.m44[1][0] * (src.m44[0][1] * src.m44[2][2] - src.m44[0][2] * src.m44[2][1]) +
		src.m44[2][0] * (src.m44[0][1] * src.m44[1][2] - src.m44[0][2] * src.m44[1][1])
		);

	SC_Memcpy(aDstMatrix, &result, 16 * sizeof(float));
}

inline float SC_Matrix::Determinant() const
{
	return	m[0] * (
		m[5] * (m[10] * m[15] - m[11] * m[14]) -
		m[9] * (m[6] * m[15] - m[7] * m[14]) +
		m[13] * (m[6] * m[11] - m[7] * m[10])
		) -
		m[4] * (
			m[1] * (m[10] * m[15] - m[11] * m[14]) -
			m[9] * (m[2] * m[15] - m[3] * m[14]) +
			m[13] * (m[2] * m[11] - m[3] * m[10])
			) +
		m[8] * (
			m[1] * (m[6] * m[15] - m[7] * m[14]) -
			m[5] * (m[2] * m[15] - m[3] * m[14]) +
			m[13] * (m[2] * m[7] - m[3] * m[6])
			) -
		m[12] * (
			m[1] * (m[6] * m[11] - m[7] * m[10]) -
			m[5] * (m[2] * m[11] - m[3] * m[10]) +
			m[9] * (m[2] * m[7] - m[3] * m[6])
			);
}