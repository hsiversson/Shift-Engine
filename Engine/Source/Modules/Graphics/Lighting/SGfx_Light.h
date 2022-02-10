#pragma once

enum class SGfx_LightType
{
	Directional,
	Spot,
	Point,
	Area
};

class SGfx_Light
{
public:
	struct GPUData
	{
		SC_Vector4 mDirectionAndIntensity;
		SC_Vector4 mPositionAndRange;
		SC_Vector4 mTintColor;
	};

public:
	SGfx_Light();
	~SGfx_Light();

	const SC_Sphere& GetBoundingSphere() const { return mBoundingSphere; }
	const SGfx_LightType& GetType() const { return mType; }
private:
	SC_Sphere mBoundingSphere;
	SGfx_LightType mType;
};

