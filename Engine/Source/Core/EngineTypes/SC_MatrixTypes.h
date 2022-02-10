#pragma once
#include "SC_Matrix.h"

class SC_ScaleMatrix : public SC_Matrix
{
public:
	SC_ScaleMatrix(float aScale)
		: SC_Matrix(
			SC_Vector4(aScale, 0.0f, 0.0f, 0.0f),
			SC_Vector4(0.0f, aScale, 0.0f, 0.0f),
			SC_Vector4(0.0f, 0.0f, aScale, 0.0f),
			SC_Vector4(0.0f, 0.0f, 0.0f, 1.0f))
	{}

	SC_ScaleMatrix(const SC_Vector& aScale)
		: SC_Matrix(
			SC_Vector4(aScale.x, 0.0f, 0.0f, 0.0f),
			SC_Vector4(0.0f, aScale.y, 0.0f, 0.0f),
			SC_Vector4(0.0f, 0.0f, aScale.z, 0.0f),
			SC_Vector4(0.0f, 0.0f, 0.0f, 1.0f))
	{}
};

class SC_TranslationMatrix : public SC_Matrix
{
public:
	SC_TranslationMatrix(const SC_Vector& aPosition)
		: SC_Matrix(
			SC_Vector4(1.0f, 0.0f, 0.0f, 0.0f),
			SC_Vector4(0.0f, 1.0f, 0.0f, 0.0f),
			SC_Vector4(0.0f, 0.0f, 1.0f, 0.0f),
			SC_Vector4(aPosition.x, aPosition.y, aPosition.z, 1.0f))
	{}
};

class SC_PerspectiveMatrix : public SC_Matrix
{
public:
	SC_PerspectiveMatrix(float aHalfFOV, float aWidth, float aHeight, float aMinZ, float aMaxZ)
		: SC_Matrix(
			SC_Vector4(1.0f / SC_Math::Tan(aHalfFOV), 0.0f, 0.0f, 0.0f),
			SC_Vector4(0.0f, aWidth / SC_Math::Tan(aHalfFOV) / aHeight, 0.0f, 0.0f),
			SC_Vector4(0.0f, 0.0f, ((aMinZ == aMaxZ) ? (1.0f) : aMaxZ / (aMaxZ - aMinZ)), 1.0f),
			SC_Vector4(0.0f, 0.0f, -aMinZ * ((aMinZ == aMaxZ) ? (1.0f) : aMaxZ / (aMaxZ - aMinZ)), 0.0f))
	{}
};

class SC_PerspectiveInvZMatrix : public SC_Matrix
{
public:
	SC_PerspectiveInvZMatrix(float aHalfFOV, float aWidth, float aHeight, float aMinZ, float aMaxZ)
		: SC_Matrix(
			SC_Vector4(1.0f / SC_Math::Tan(aHalfFOV), 0.0f, 0.0f, 0.0f),
			SC_Vector4(0.0f, aWidth / SC_Math::Tan(aHalfFOV) / aHeight, 0.0f, 0.0f),
			SC_Vector4(0.0f, 0.0f, ((aMinZ == aMaxZ) ? 0.0f : aMinZ / (aMinZ - aMaxZ)), 1.0f),
			SC_Vector4(0.0f, 0.0f, ((aMinZ == aMaxZ) ? aMinZ : -aMaxZ * aMinZ / (aMinZ - aMaxZ)), 0.0f))
	{}
};

class SC_OrthogonalMatrix : public SC_Matrix
{
public:
	SC_OrthogonalMatrix(float aWidth, float aHeight, float aZScale, float aZOffset)
		: SC_Matrix(
			SC_Vector4((aWidth != 0.0f) ? (1.0f / aWidth) : 1.0f, 0.0f, 0.0f, 0.0f),
			SC_Vector4(0.0f, (aHeight != 0.0f) ? (1.0f / aHeight) : 1.f, 0.0f, 0.0f),
			SC_Vector4(0.0f, 0.0f, aZScale, 0.0f),
			SC_Vector4(0.0f, 0.0f, aZOffset * aZScale, 1.0f))
	{}
};

class SC_OrthogonalInvZMatrix : public SC_Matrix
{
public:
	SC_OrthogonalInvZMatrix(float aWidth, float aHeight, float aZScale, float aZOffset)
		: SC_Matrix(
			SC_Vector4((aWidth != 0.0f) ? (1.0f / aWidth) : 1.0f, 0.0f, 0.0f, 0.0f),
			SC_Vector4(0.0f, (aHeight != 0.0f) ? (1.0f / aHeight) : 1.f, 0.0f, 0.0f),
			SC_Vector4(0.0f, 0.0f, -aZScale, 0.0f),
			SC_Vector4(0.0f, 0.0f, 1.0f - aZOffset * aZScale, 1.0f))
	{}
};

//class SRotationMatrix : public SMatrix
//{
//public:
//	SRotationMatrix(const SVector& aRotationVec)
//	{
//		m44[0][0] = 1.0f;
//		m44[0][1] = 0.0f;
//		m44[0][2] = 0.0f;
//		m44[0][3] = 0.0f;
//		m44[1][0] = 0.0f;
//		m44[1][1] = 1.0f;
//		m44[1][2] = 0.0f;
//		m44[1][3] = 0.0f;
//		m44[2][0] = 0.0f;
//		m44[2][1] = 0.0f;
//		m44[2][2] = 1.0f;
//		m44[2][3] = 0.0f;
//		m44[3][0] = 0.0f;
//		m44[3][1] = 0.0f;
//		m44[3][2] = 0.0f;
//		m44[3][3] = 1.0f;
//		SetRotation(aRotationVec);
//	}
//};

class SC_LookAtMatrix : public SC_Matrix
{
public:
	SC_LookAtMatrix(const SC_Vector& aFromPosition, const SC_Vector& aLookAtPosition, const SC_Vector& aUpVector)
	{
		const SC_Vector zAxis = (aLookAtPosition - aFromPosition).GetNormalized();
		const SC_Vector xAxis = aUpVector.Cross(zAxis).GetNormalized();
		const SC_Vector yAxis = zAxis.Cross(xAxis);

		for (uint32 row = 0; row < 3; row++)
		{
			uint32 offset = 4 * row;

			m[0 + offset] = (&xAxis.x)[row];
			m[1 + offset] = (&yAxis.x)[row];
			m[2 + offset] = (&zAxis.x)[row];
			m[3 + offset] = 0.0f;
		}


		uint32 offset = 4 * 3;
		m[0 + offset] = -aFromPosition.Dot(xAxis);
		m[1 + offset] = -aFromPosition.Dot(yAxis);
		m[2 + offset] = -aFromPosition.Dot(zAxis);
		m[3 + offset] = 1.0f;
	}
};