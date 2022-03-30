#pragma once
#include "Graphics/View/SGfx_Camera.h"
#include "SGfx_ShadowConstants.h"

struct SGfx_ViewData;
class SGfx_View;

class SGfx_CascadedShadowMap
{
public:
	struct Settings
	{
		Settings() 
			: mResolution(2048)
			, mMaxDistance(400.0f)
			, mSplitFactor(0.93f)
#if IS_DEBUG_BUILD
			, mDebugDrawFrustums(false)
			, mLockShadowView(false)
#endif
		{}

		static constexpr uint32 gNumCascades = 4;
		uint32 mResolution;
		float mMaxDistance;
		float mSplitFactor;
#if IS_DEBUG_BUILD
		bool mDebugDrawFrustums;
		bool mLockShadowView;
#endif
	};

	struct Cascade
	{
		Cascade() : mNeedsCacheUpdate(true) {}

		SGfx_Camera mCamera;
		SC_Ref<SR_DepthStencil> mTarget;
		SC_Ref<SR_Texture> mTexture;

		SC_Sphere mMBSCache;
		SC_Vector mCameraToMBSCenterCache;
		SC_Vector mOldPosition;

		bool mNeedsCacheUpdate;
	};

public:
	SGfx_CascadedShadowMap();
	~SGfx_CascadedShadowMap();

	bool Init(const Settings& aSettings);

	void UpdateViews(SGfx_View* aMainView);

	void Generate(SR_CommandList* aCmdList, const SGfx_ViewData& aRenderData);

	const Cascade& GetCascade(uint32 aIndex) const;
	Settings& GetSettings();
	const Settings& GetSettings() const;

	SC_Vector4 GetSplitPoints() const;
	const SC_IntVector4 GetCSMDescriptorIndices() const;

private:
	bool InitProjections();
	bool CreateTargets();

	void RenderCascade(SR_CommandList* aCmdList, uint32 aIndex, const SGfx_ViewData& aRenderData);

	void CalculateSplitPoints(float aNear, float aFar);

private:
	Settings mSettings;
	SC_Array<Cascade> mCascades;
	float mSplitPoints[Settings::gNumCascades + 1];
	SC_Ref<SR_TextureResource> mCSMResource;
	SC_Array<SC_Ref<SR_BufferResource>> mDrawInfoBuffers[4];
	SC_Ref<SR_BufferResource> mSceneConstantsBuffer[4];

#if IS_DEBUG_BUILD
	SGfx_Camera mLockedViewCamera;
	bool mWasLocked;
#endif
};

class SGfx_ShadowSystem
{
public:
	SGfx_ShadowSystem();
	~SGfx_ShadowSystem();

	SC_Ref<SGfx_View> CreateShadowView();
	void GenerateShadowMaps(const SGfx_ViewData& aRenderData);

	SGfx_CascadedShadowMap* GetCSM() const; 

	SGfx_ShadowConstants GetShadowConstants() const;

private:
	SC_UniquePtr<SGfx_CascadedShadowMap> mCSM;
};

