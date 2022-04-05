#pragma once
#include "Graphics/Misc/SGfx_Surface.h"

class SGfx_View;
class SGfx_Camera;
struct SGfx_ViewData;
class SR_BufferResource;
class SR_ShaderState;
class SR_Texture;
class SR_CommandList;

struct alignas(16) SGfx_SkyAtmosphereConstants
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

struct alignas(16) SGfx_SkyIrradianceConstants
{
	uint32 mIrradianceBrdfLUTDescriptorIndex;
	uint32 mDiffuseIrradianceMapDescriptorIndex;
	uint32 mPreFilteredSpecularIrradianceMapDescriptorIndex;
	uint32 mNumPreFilteredIrradianceMapMips;
};

class SGfx_Sky
{
public:
	SGfx_Sky();
	~SGfx_Sky();

	bool Init();

	void UpdateConstants(const SGfx_Camera& aWorldCamera);
	const SGfx_SkyAtmosphereConstants& GetSkyAtmosphereConstants() const;
	const SGfx_SkyIrradianceConstants& GetSkyIrradianceConstants() const;

	void Render(SR_CommandList* aCmdList);
	void ComputeSkyIrradiance(SR_CommandList* aCmdList);
	void ComputeSkyAtmosphereLUTs(SR_CommandList* aCmdList);

private:
	bool InitSkySphere();
	bool InitSkyAtmosphere();
	bool InitSkyIrradianceMaps();

private:
	SGfx_SkyAtmosphereConstants mSkyAtmosphereConstants;
	SGfx_SkyIrradianceConstants mSkyIrradianceConstants;

	SC_Ref<SR_BufferResource> mSphereVertexBuffer;
	SC_Ref<SR_BufferResource> mSphereIndexBuffer;
	SC_Ref<SR_ShaderState> mSphereShader;

	SC_Ref<SR_Texture> mDiffuseIrradianceMap;
	SC_Ref<SR_Texture> mPreFilteredSpecularIrradiaceMap;
	SC_Ref<SR_Texture> mIrradianceBrdfLUT;

	SGfx_Surface mTransmittanceLUT;
	SC_Ref<SR_ShaderState> mComputeTransmittanceLUTShader;
	SC_Ref<SR_BufferResource> mTransmittanceLUTConstantBuffer;

	SGfx_Surface mSkyViewLUT;
	SC_Ref<SR_ShaderState> mComputeSkyViewLUTShader;
	SC_Ref<SR_BufferResource> mSkyViewLUTConstantBuffer;
};