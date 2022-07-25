#pragma once
#include "Graphics/Misc/SGfx_Surface.h"

class SR_ShaderState;
class SR_CommandList;
class SGfx_View;
struct SGfx_ViewData;

struct SGfx_PostEffectsSettings
{
	// Motion Blur

	// Exposure

	// Bloom
	float mBloomThreshold;
	float mBloomIntensity;
	bool mBloomEnabled;
};

class SGfx_PostEffects
{
public:
	SGfx_PostEffects();
	~SGfx_PostEffects();

	bool Init();
	void Render(SGfx_View* aView, SR_Texture* aScreenColor);

	SR_Texture* GetBloomTexture() const;

private:

	struct AverageLuminance
	{
		bool mEnabled : 1;
	};

	struct Bloom
	{
		struct Constants
		{
			float mBrightnessThreshold;
		} mConstants;

		SC_Ref<SR_ShaderState> mBrightnessFilterShader;
		SC_Ref<SR_ShaderState> mUpsampleShader;

		SC_Ref<SR_Texture> mResult;

		bool mEnabled : 1;
	};

	struct LensFlare
	{

		bool mEnabled : 1;
	};

	struct ChromaticAbberation
	{

		bool mEnabled : 1;
	};

	struct Tonemap
	{
		struct Constants
		{

		} mConstants;

		SC_Ref<SR_ShaderState> mShader;
		bool mEnabled : 1;
	};


private:

	void RenderAverageLuminance();

	void RenderBloom(SGfx_View* aView, SR_Texture* aScreenColor);
	void RenderBloomMipRecursive(const SGfx_ViewData& aRenderData, SR_TempTexture& aOutMip, SR_Texture* aInMip);
	void UpsampleBloomMip(SR_TempTexture& aOutMip, SR_Texture* aFullMip, SR_TempTexture& aDownsampledMip);

	SR_TempTexture Downsample(SR_Texture* aSource);

	void RenderLensFlare();
	void RenderChromaticAbberation();
	void RenderTonemap(SGfx_View* aView);

	AverageLuminance mAverageLuminanceData;
	Bloom mBloomData;
	LensFlare mLensFlareData;
	ChromaticAbberation mChromaticAbberationData;
	Tonemap mTonemapData;

	SC_Ref<SR_ShaderState> mDownsampleShader;

public:
	static bool gEnableTAA;
	static bool gEnableBloom;
};