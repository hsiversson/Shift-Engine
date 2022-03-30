#pragma once

enum class SGfx_LightType
{
	Directional,
	Point,
	Spot
};

enum class SGfx_LightUnit
{
	None,
	Candelas,
	Lumens
};

class SGfx_Light
{
public:
	struct LocalLightShaderData
	{
		SC_Vector mDirection;
		float mInvRange;

		SC_Vector mPosition;
		float mRange;

		SC_Vector mColoredIntensity;
		uint32 mType;

		SC_Vector2 mSpotAngles;
		uint32 mShadowMapDescriptorIndex;
		float mSoftSourceRadius;

		float mSourceRadius;
		float _unused0[3];
	};

public:
	SGfx_Light(const SGfx_LightType& aType, const SGfx_LightUnit& aLightUnit);
	virtual ~SGfx_Light();

	const SC_Sphere& GetBoundingSphere() const { return mBoundingSphere; }
	void SetBoundingSphere(const SC_Sphere& aSphere) { mBoundingSphere = aSphere; }
	const SGfx_LightType& GetType() const { return mType; }

	const SC_Vector4& GetColor() const;
	void SetColor(const SC_Vector4& aColor);

	void SetLightUnit(const SGfx_LightUnit& aLightUnit);
	const SGfx_LightUnit& GetLightUnit() const;

	virtual void SetIntensity(float aBrightness);
	virtual float GetIntensity() const;

	virtual LocalLightShaderData GetShaderData() const;

protected:
	SC_Sphere mBoundingSphere;

	SC_Vector4 mColor;
	float mIntensity; // Stored using the mLightUnit uint.

	SGfx_LightUnit mLightUnit;
	SGfx_LightType mType;
};


class SGfx_DirectionalLight : public SGfx_Light
{
public:
	SGfx_DirectionalLight();
	~SGfx_DirectionalLight();

	void SetIntensity(float aIntensity) override;
	float GetIntensity() const override;

	virtual LocalLightShaderData GetShaderData() const override;
private:
};

class SGfx_PointLight : public SGfx_Light
{
public:
	SGfx_PointLight();
	~SGfx_PointLight();

	void SetIntensity(float aIntensity) override;
	float GetIntensity() const override;

	void SetPosition(const SC_Vector& aPosition);
	const SC_Vector& GetPosition() const;

	void SetRange(float aRange);
	float GetRange() const;

	void SetSourceRadius(float aRadius);
	float GetSourceRadius() const;

	void SetSourceRadiusSoft(float aRadius);
	float GetSourceRadiusSoft() const;

	virtual LocalLightShaderData GetShaderData() const override;

private:
	SC_Vector mPosition;
	float mRange;

	float mSourceRadius;
	float mSourceRadiusSoft;
};

class SGfx_SpotLight : public SGfx_Light
{
public:
	SGfx_SpotLight();
	~SGfx_SpotLight();

	void SetIntensity(float aIntensity) override;
	float GetIntensity() const override;

	void SetPosition(const SC_Vector& aPosition);
	const SC_Vector& GetPosition() const;

	void SetDirection(const SC_Vector& aDirection);
	const SC_Vector& GetDirection() const;

	void SetRange(float aRange);
	float GetRange() const;

	void SetInnerAngle(float aAngle);
	float GetInnerAngle() const;

	void SetOuterAngle(float aAngle);
	float GetOuterAngle() const;

	SC_Vector2 GetSpotAngles() const;

	float GetHalfConeAngle() const;
	float GetCosHalfConeAngle() const;

	void SetSourceRadius(float aRadius);
	float GetSourceRadius() const;

	void SetSourceRadiusSoft(float aRadius);
	float GetSourceRadiusSoft() const;

	virtual LocalLightShaderData GetShaderData() const override;

private:
	SC_Vector mPosition;
	SC_Vector mDirection;

	float mRange;
	float mInnerAngle;
	float mOuterAngle;

	float mSourceRadius;
	float mSourceRadiusSoft;
};