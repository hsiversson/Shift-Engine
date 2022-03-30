#pragma once
#include "Graphics/Misc/SGfx_Surface.h"

class SR_Texture;
class SR_BufferResource;
class SR_ShaderState;
class SR_CommandList;
class SGfx_Camera;

struct alignas(16) SGfx_ScatteringConstants
{
	float mPlanetRadiusKm; // in Kilometers
	float mPlanetAtmosphereRadiusKm;  // in Kilometers
	uint32 mTransmittanceLUTDescriptorIndex;
	uint32 mMultiScatteringLUTDescriptorIndex;

	SC_Vector mRayleighScattering;
	float mRayleighDensityExpScale;

	SC_Vector mMieScattering;
	float _unused0;

	SC_Vector mMieAbsorption;
	float mMiePhaseG;

	SC_Vector mMieExtinction;
	float mMieDensityExpScale;

	SC_Vector mAbsorptionExtinction;
	float mAbsorptionDensity0LayerWidth;

	float mAbsorptionDensity0ConstantTerm;
	float mAbsorptionDensity0LinearTerm;
	float mAbsorptionDensity1ConstantTerm;
	float mAbsorptionDensity1LinearTerm;

	SC_Vector4 mPlanetCenterAndViewHeight;

	SC_Vector mVirtualSkyCameraPosition;
	float _unused1;
};

struct alignas(16) SGfx_EnvironmentConstants
{
	SC_Vector mSunLightDirection;
	float mSunLightIntensity;

	SC_Vector mSunLightColor;
	uint32 mEnvironmentalIrradianceMapDescriptorIndex;

	uint32 mPreFilteredEnvironmentMapDescriptorIndex;
	uint32 mEnvironmentBrdfLUTDescriptorIndex;
	uint32 mPreFilteredEnvironmentMapMips;
	uint32 _pad;

	SGfx_ScatteringConstants mScatteringConstants;
};

class SGfx_Skysphere
{
public:
	SGfx_Skysphere();
	~SGfx_Skysphere();

	bool Init();

	void Render(SR_CommandList* aCmdList);

private:
	SC_Ref<SR_BufferResource> mVertexBuffer;
	SC_Ref<SR_BufferResource> mIndexBuffer;
	SC_Ref<SR_ShaderState> mShader;
};

class SGfx_Environment
{
public:
	SGfx_Environment();
	~SGfx_Environment();

	const SGfx_EnvironmentConstants& GetConstants() const;

	SC_Vector& GetSunDirection();
	const SC_Vector& GetSunDirection() const;
	SC_Vector& GetSunColor();
	const SC_Vector& GetSunColor() const;
	float& GetSunIntensity();
	const float& GetSunIntensity() const;

	void UpdateConstants(const SGfx_Camera& aWorldCamera);

	SGfx_Skysphere* GetSkysphere() const { return mSkysphere.get(); }

	void ComputeScatteringLUTs(SR_CommandList* aCmdList);

	SR_Texture* GetTransmittanceLUT() const;
	SR_Texture* GetSkyViewLUT() const;

private:
	SGfx_EnvironmentConstants mConstants;

	SC_Ref<SR_Texture> mEnvironmentalIrradianceMap;
	SC_Ref<SR_Texture> mPreFilteredEnvironmentMap;
	SC_Ref<SR_Texture> mEnvironmentalBrdfLUT;

	SGfx_Surface mTransmittanceLUT;
	SC_Ref<SR_ShaderState> mComputeTransmittanceLUTShader;
	SC_Ref<SR_BufferResource> mTransmittanceLUTConstantBuffer;

	SGfx_Surface mSkyViewLUT;
	SC_Ref<SR_ShaderState> mComputeSkyViewLUTShader;
	SC_Ref<SR_BufferResource> mSkyViewLUTConstantBuffer;
	

	SC_UniquePtr<SGfx_Skysphere> mSkysphere;

};

