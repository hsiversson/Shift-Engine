#include "SGfx_Light.h"

SGfx_Light::SGfx_Light(const SGfx_LightType& aType, const SGfx_LightUnit& aLightUnit)
	: mIntensity(0.0f)
	, mCastShadow(false)
	, mLightUnit(aLightUnit)
	, mType(aType)
{
}

SGfx_Light::~SGfx_Light()
{
}

const SC_Vector4& SGfx_Light::GetColor() const
{
	return mColor;
}

void SGfx_Light::SetColor(const SC_Vector4& aColor)
{
	mColor = aColor;
}

void SGfx_Light::SetLightUnit(const SGfx_LightUnit& aLightUnit)
{
	mLightUnit = aLightUnit;
}

const SGfx_LightUnit& SGfx_Light::GetLightUnit() const
{
	return mLightUnit;
}

void SGfx_Light::SetIntensity(float aIntensity)
{
	mIntensity = aIntensity;
}

float SGfx_Light::GetIntensity() const
{
	return mIntensity;
}

SGfx_Light::LocalLightShaderData SGfx_Light::GetShaderData() const
{
	return LocalLightShaderData();
}

SGfx_DirectionalLight::SGfx_DirectionalLight()
	: SGfx_Light(SGfx_LightType::Directional, SGfx_LightUnit::Lumens)
{

}

SGfx_DirectionalLight::~SGfx_DirectionalLight()
{

}

void SGfx_DirectionalLight::SetIntensity(float aIntensity)
{
	SGfx_Light::SetIntensity(aIntensity);
}

float SGfx_DirectionalLight::GetIntensity() const
{
	return SGfx_Light::GetIntensity();
}

SGfx_DirectionalLight::LocalLightShaderData SGfx_DirectionalLight::GetShaderData() const
{
	return LocalLightShaderData();
}

SGfx_PointLight::SGfx_PointLight()
	: SGfx_Light(SGfx_LightType::Point, SGfx_LightUnit::Candelas)
{

}

SGfx_PointLight::~SGfx_PointLight()
{

}

void SGfx_PointLight::SetIntensity(float aIntensity)
{
	//switch (mLightUnit)
	//{
	//case SGfx_LightUnit::Candelas:
	//	SGfx_Light::SetIntensity(aIntensity / (100.0f * 100.0f));
	//	break;
	//case SGfx_LightUnit::Lumens:
	//	SGfx_Light::SetIntensity(aIntensity / (100.0f * 100.0f / 4.0f / SC_Math::PI));
	//	break;
	//default:
	//	SGfx_Light::SetIntensity(aIntensity / 16.f);
	//}
	SGfx_Light::SetIntensity(aIntensity);
}

float SGfx_PointLight::GetIntensity() const
{
	float Intensity = SGfx_Light::GetIntensity();
	switch (mLightUnit)
	{
	case SGfx_LightUnit::Candelas:
		return Intensity;
	case SGfx_LightUnit::Lumens:
		Intensity /= (4.0f * SC_Math::PI);
		break;
	default:
		Intensity *= 16.0f;
	}

	return Intensity;
}

void SGfx_PointLight::SetPosition(const SC_Vector& aPosition)
{
	mPosition = aPosition;
}

const SC_Vector& SGfx_PointLight::GetPosition() const
{
	return mPosition;
}

void SGfx_PointLight::SetRange(float aRange)
{
	mRange = aRange;
}

float SGfx_PointLight::GetRange() const
{
	return mRange;
}

void SGfx_PointLight::SetSourceRadius(float aRadius)
{
	mSourceRadius = aRadius;
}

float SGfx_PointLight::GetSourceRadius() const
{
	return mSourceRadius;
}

void SGfx_PointLight::SetSourceRadiusSoft(float aRadius)
{
	mSourceRadiusSoft = aRadius;
}

float SGfx_PointLight::GetSourceRadiusSoft() const
{
	return mSourceRadiusSoft;
}

SGfx_PointLight::LocalLightShaderData SGfx_PointLight::GetShaderData() const
{
	SGfx_Light::LocalLightShaderData shaderData;
	shaderData.mColoredIntensity = GetColor() * GetIntensity();
	shaderData.mDirection = SC_Vector(0);
	shaderData.mPosition = GetPosition();
	shaderData.mRange = GetRange();
	shaderData.mInvRange = 1.0f / shaderData.mRange;
	shaderData.mSourceRadius = GetSourceRadius();
	shaderData.mSoftSourceRadius = GetSourceRadiusSoft();
	shaderData.mSpotAngles = SC_Vector2(0);
	shaderData.mCastShadow = mCastShadow;
	shaderData.mType = static_cast<uint32>(SGfx_LightType::Point);

	return shaderData;
}

SGfx_SpotLight::SGfx_SpotLight()
	: SGfx_Light(SGfx_LightType::Spot, SGfx_LightUnit::Candelas)
{

}

SGfx_SpotLight::~SGfx_SpotLight()
{

}

void SGfx_SpotLight::SetIntensity(float aIntensity)
{
	SGfx_Light::SetIntensity(aIntensity);
}

float SGfx_SpotLight::GetIntensity() const
{
	float Intensity = SGfx_Light::GetIntensity();
	switch (mLightUnit)
	{
	case SGfx_LightUnit::Candelas:
		return Intensity;
	case SGfx_LightUnit::Lumens:
		Intensity /= (2.0f * SC_Math::PI * (1.0f - GetCosHalfConeAngle()));
		break;
	default:
		Intensity *= 16.0f;
	}

	return Intensity;
}

void SGfx_SpotLight::SetPosition(const SC_Vector& aPosition)
{
	mPosition = aPosition;
}

const SC_Vector& SGfx_SpotLight::GetPosition() const
{
	return mPosition;
}

void SGfx_SpotLight::SetDirection(const SC_Vector& aDirection)
{
	mDirection = aDirection.GetNormalized();
}

const SC_Vector& SGfx_SpotLight::GetDirection() const
{
	return mDirection;
}

void SGfx_SpotLight::SetRange(float aRange)
{
	mRange = aRange;
}

float SGfx_SpotLight::GetRange() const
{
	return mRange;
}

void SGfx_SpotLight::SetInnerAngle(float aAngle)
{
	mInnerAngle = aAngle;
}

float SGfx_SpotLight::GetInnerAngle() const
{
	return mInnerAngle;
}

void SGfx_SpotLight::SetOuterAngle(float aAngle)
{
	mOuterAngle = aAngle;
}

float SGfx_SpotLight::GetOuterAngle() const
{
	return mOuterAngle;
}

SC_Vector2 SGfx_SpotLight::GetSpotAngles() const
{
	const float clampedInnerConeAngle = SC_Clamp(mInnerAngle, 0.0f, 89.0f) * SC_Math::PI / 180.0f;
	const float clampedOuterConeAngle = SC_Clamp(mOuterAngle * SC_Math::PI / 180.0f, clampedInnerConeAngle + 0.001f, 89.0f * SC_Math::PI / 180.0f + 0.001f);
	const float cosOuterCone = SC_Math::Cos(clampedOuterConeAngle);
	const float cosInnerCone = SC_Math::Cos(clampedInnerConeAngle);
	const float invCosConeDifference = 1.0f / (cosInnerCone - cosOuterCone);
	return SC_Vector2(cosOuterCone, invCosConeDifference);
}

float SGfx_SpotLight::GetHalfConeAngle() const
{
	const float clampedInnerConeAngle = SC_Clamp(mInnerAngle, 0.0f, 89.0f) * SC_Math::PI / 180.0f;
	const float clampedOuterConeAngle = SC_Clamp(mOuterAngle * SC_Math::PI / 180.0f, clampedInnerConeAngle + 0.001f, 89.0f * SC_Math::PI / 180.0f + 0.001f);
	return clampedOuterConeAngle;
}

float SGfx_SpotLight::GetCosHalfConeAngle() const
{
	return SC_Math::Cos(GetHalfConeAngle());
}

void SGfx_SpotLight::SetSourceRadius(float aRadius)
{
	mSourceRadius = aRadius;
}

float SGfx_SpotLight::GetSourceRadius() const
{
	return mSourceRadius;
}

void SGfx_SpotLight::SetSourceRadiusSoft(float aRadius)
{
	mSourceRadiusSoft = aRadius;
}

float SGfx_SpotLight::GetSourceRadiusSoft() const
{
	return mSourceRadiusSoft;
}

SGfx_SpotLight::LocalLightShaderData SGfx_SpotLight::GetShaderData() const
{
	SGfx_Light::LocalLightShaderData shaderData;
	shaderData.mColoredIntensity = GetColor() * GetIntensity();
	shaderData.mDirection = GetDirection();
	shaderData.mPosition = GetPosition();
	shaderData.mRange = GetRange();
	shaderData.mInvRange = 1.0f / shaderData.mRange;
	shaderData.mSourceRadius = GetSourceRadius();
	shaderData.mSoftSourceRadius = GetSourceRadiusSoft();
	shaderData.mSpotAngles = GetSpotAngles();
	shaderData.mCastShadow = mCastShadow;
	shaderData.mType = static_cast<uint32>(SGfx_LightType::Spot);

	return shaderData;
}
