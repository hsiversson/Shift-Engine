#pragma once
#include "Graphics/Lighting/Raytracing/SGfx_Reflections.h"

class SGfx_View;
struct SGfx_ViewData;
class SGfx_DDGI
{
public:
	bool Init();

	void GetConstants(SGfx_ViewData& aPrepareData);

	void RenderDiffuse(SGfx_View* aView);
	void RenderSpecular(SGfx_View* aView);

	void DebugRenderProbes(SGfx_View* aView);

public:
	struct ProbeGridProperties
	{
		SC_IntVector mNumProbes;
		SC_Vector mStartPosition;
		float mDistanceBetweenProbes;
		float mMaxDistance;
		uint32 mRaysPerProbe;
		uint32 mIrradianceProbeSideLength;
		uint32 mDepthProbeSideLength;

		float mGlobalRoughnessMultiplier;
		float mNormalBias;
		float mHistoryFactor;
		float mDepthSharpness;
		float mEnergyConservation;

		bool mInfiniteBounces;
		bool mDiffuseEnabled;
		bool mSpecularEnabled;
		bool mVisualizeProbes;
	};

	struct alignas(16) Constants
	{
		SC_Vector mGridStartPosition;
		float mMaxDistance;

		SC_Vector mGridStep;
		float mDepthSharpness;

		SC_IntVector mNumProbes;
		float mHistoryFactor;

		float mNormalBias;
		float mEnergyConservation;
		uint32 mEnableVisibilityTesting;
		uint32 mRaysPerProbe;

		SC_IntVector2 mIrradianceTextureDimensions;
		uint32 mIrradianceProbeSideLength;
		uint32 mIrradianceTextureDescriptorIndex;

		SC_IntVector2 mDepthTextureDimensions;
		uint32 mDepthProbeSideLength;
		uint32 mDepthTextureDescriptorIndex;

		uint32 mDiffuseGIDescriptorIndex;
		uint32 mReflectionsDescriptorIndex;
		uint32 _unused[2];

		float mGlobalRoughnessMultiplier;
		uint32 mInfiniteBounces;
		uint32 mDiffuseEnabled;
		uint32 mSpecularEnabled;
	};
	ProbeGridProperties mProbeGridProperties;

private:
	bool CreateShaders();
	bool CreateTextures();
	void UpdateResources();
	bool InitDebugResources();

	SC_Ref<SR_ShaderState> mTraceRaysShader;
	SC_Ref<SR_ShaderState> mUpdateProbesShader[2];
	SC_Ref<SR_ShaderState> mUpdateProbesBorderShader[2];
	SC_Ref<SR_ShaderState> mSampleProbesShader;
	SC_Ref<SR_ShaderState> mReflectionsTraceRaysShader;

	SC_Ref<SR_Texture> mProbeIrradianceTexture;
	SC_Ref<SR_Texture> mProbeIrradianceTextureRW;
	SC_Ref<SR_Texture> mProbeDepthTexture;
	SC_Ref<SR_Texture> mProbeDepthTextureRW;
	SC_Ref<SR_Texture> mTraceRaysRadianceTexture;
	SC_Ref<SR_Texture> mTraceRaysRadianceTextureRW;
	SC_Ref<SR_Texture> mTraceRaysDirectionDepthTexture;
	SC_Ref<SR_Texture> mTraceRaysDirectionDepthTextureRW;
	SC_Ref<SR_Texture> mDiffuseGITexture;
	SC_Ref<SR_Texture> mDiffuseGITextureRW;
	SC_Ref<SR_Texture> mReflectionsTexture;
	SC_Ref<SR_Texture> mReflectionsTextureRW;

	// DEBUG
	SC_Ref<SR_ShaderState> mDebugSphereShader;
	SC_Ref<SR_BufferResource> mSphereVertexBuffer;
	SC_Ref<SR_BufferResource> mSphereIndexBuffer;
};


class SGfx_Raytracing
{
public:
	SGfx_Raytracing();
	~SGfx_Raytracing();

	bool Init();

	SGfx_DDGI* GetDDGI();
	SGfx_Reflections* GetReflections();

private:
	SGfx_DDGI mDDGI;
	SGfx_Reflections mReflections;
};

