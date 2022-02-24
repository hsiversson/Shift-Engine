#pragma once

class SC_Quaternion
{
public:
	union 
	{
		SC_Vector4 mVector;
		struct
		{
			float x, y, z, w;
		};
	};

public:
	SC_Quaternion();
	SC_Quaternion(float aX, float aY, float aZ, float aW);
	SC_Quaternion(const SC_Vector4& aVector);
	SC_Quaternion(const SC_Matrix& aMatrix);
	~SC_Quaternion();

	static SC_Quaternion Identity();

	bool IsNormalized() const;
	void Normalize();
	SC_Quaternion GetNormalized() const;

	// Matrix
	SC_Matrix AsMatrix() const;
	void FromMatrix(const SC_Matrix& aMatrix);

	// Euler Angles
	SC_Vector AsEulerAngles(bool aAsDegrees = true) const;
	static SC_Quaternion FromEulerAngles(const SC_Vector& aAngles, bool aIsDegrees = true);

	// Rotation
	static SC_Quaternion CreateRotation(const SC_Vector& aAxis, float aAngle);

	// Operators
	SC_Quaternion operator*(const SC_Quaternion& aOther) const;
	SC_Quaternion operator*=(const SC_Quaternion& aOther);
};
