#pragma once

class SR_Texture;
class SR_BufferResource;
class SR_ShaderState;
class SR_CommandList;

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
};

class SGfx_Skybox
{
public:
	SGfx_Skybox();
	~SGfx_Skybox();

	bool Init();

	void Render(SR_CommandList* aCmdList);

private:
	SC_Ref<SR_Texture> mSkyCubeMap;
	SC_Ref<SR_BufferResource> mCubeVertices;
	SC_Ref<SR_BufferResource> mCubeIndices;
	SC_Ref<SR_ShaderState> mCubeShader;
	SC_Ref<SR_BufferResource> mDrawInfoBuffer;
};

class SGfx_Environment
{
public:
	SGfx_Environment();
	~SGfx_Environment();

	const SGfx_EnvironmentConstants& GetConstants() const;

	SGfx_Skybox* GetSkybox() const { return mSkybox.get(); }

private:
	SGfx_EnvironmentConstants mConstants;

	SC_Ref<SR_Texture> mEnvironmentalIrradianceMap;
	SC_Ref<SR_Texture> mPreFilteredEnvironmentMap;
	SC_Ref<SR_Texture> mEnvironmentalBrdfLUT;

	SC_UniquePtr<SGfx_Skybox> mSkybox;

};

