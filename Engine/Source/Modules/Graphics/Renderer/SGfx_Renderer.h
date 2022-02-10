#pragma once
#include "Graphics/Misc/SGfx_Surface.h"
#include "RenderCore/Defines/SR_Format.h"
#include "RenderCore/RenderTasks/SR_RenderTaskManager.h"

class SR_CommandList;
class SR_ShaderState;
class SR_BufferResource;
class SR_DepthStencil;
class SR_TextureResource;
class SR_Texture;
class SR_RenderTarget;
class SGfx_View;
class SGfx_ShadowSystem;
class SGfx_Raytracing;
class SGfx_Sky;
class SGfx_LightCulling;
class SGfx_ReflectionProbe;
class SGfx_AmbientOcclusion;
class SGfx_PostEffects;

/**
 * 
 *		Default scene renderer.
 *		Renders views with a clustered forward path.
 * 
 */
class SGfx_Renderer
{
public:
	struct Settings
	{
		Settings()
			: mEnableTemporalAA(true)
		{}

		bool mEnableTemporalAA;
	};

public:
	SGfx_Renderer();
	~SGfx_Renderer();

	bool Init();

	// Called from MainThread to kick off render tasks
	void RenderView(SGfx_View* aView);

	SC_Vector2 GetJitter(const SC_IntVector2& aTargetResolution) const;

	SGfx_ShadowSystem* GetShadowMapSystem() const;
	SGfx_AmbientOcclusion* GetAmbientOcclusion() const;

	const SC_Ref<SR_Texture>& GetScreenColor() const;
	SR_Texture* GetAoTex() const;

	Settings& GetSettings();
	const Settings& GetSettings() const;

private:

	void SubmitGraphicsTask(SR_RenderTaskFunctionSignature aTask, SR_TaskEvent* aEvent);
	void SubmitGraphicsTask(SR_RenderTaskFunctionSignature aTask, const SC_UniquePtr<SR_TaskEvent>& aEvent);
	void SubmitComputeTask(SR_RenderTaskFunctionSignature aTask, SR_TaskEvent* aEvent);
	void SubmitComputeTask(SR_RenderTaskFunctionSignature aTask, const SC_UniquePtr<SR_TaskEvent>& aEvent);
	void SubmitCopyTask(SR_RenderTaskFunctionSignature aTask, SR_TaskEvent* aEvent);
	void SubmitCopyTask(SR_RenderTaskFunctionSignature aTask, const SC_UniquePtr<SR_TaskEvent>& aEvent);

	/////////////////////////
	// Render Tasks
	void PreRenderUpdates();

#if ENABLE_RAYTRACING
	void ComputeRaytracingScene();
#endif

	void RenderShadows();
	void RenderPrePass(); // Depth, Visibility Buffer

	void ComputeHierarchicalZ(); // Generate HZB Chain
	void ComputeLightCulling();
	void ComputeAmbientOcclusion();

	void RenderOpaque();

	void ComputeParticles();

	void RenderVolumetrics();
	void RenderTranslucency();

	void RenderUI();

	void ComputePostEffects();
	/////////////////////////

	SC_UniquePtr<SGfx_LightCulling> mLightCulling;
	SC_UniquePtr<SGfx_ShadowSystem> mShadowMapSystem;
	SC_UniquePtr<SGfx_Raytracing> mRaytracingSystem; 
	SC_UniquePtr<SGfx_Sky> mSky; 
	SC_UniquePtr<SGfx_AmbientOcclusion> mAmbientOcclusion;
	SC_UniquePtr<SGfx_PostEffects> mPostEffects;

	// Temp
	SC_Ref<SR_ShaderState> mShader;
	SC_Array<SC_Ref<SR_BufferResource>> mDrawInfoBuffers[2];
	SC_Ref<SR_BufferResource> mViewConstantsBuffer;

	SC_Array<SC_Ref<SR_BufferResource>> mPostEffectCBuffers;
	SC_Ref<SR_ShaderState> mTonemapShader;
	SC_Ref<SR_ShaderState> mTAAResolveShader;
	SC_Ref<SR_ShaderState> mCopyShader;
	SC_Ref<SR_DepthStencil> mDepthStencil;
	SC_Ref<SR_Texture> mDepthStencilSRV;
	SGfx_Surface mSceneColor;
	SGfx_Surface mSceneColor2;
	SGfx_Surface mHistoryBuffer;
	SGfx_Surface mScreenColor;
	SGfx_Surface mMotionVectors;
	SC_Ref<SR_Buffer> mRaytracingScene;
	//

	SGfx_View* mCurrentView;

	SC_Ref<SGfx_ReflectionProbe> mReflectionProbe;

	SC_Array<SC_Event*> mSubmittedTaskEvents;
	SC_Event* mLatestTaskEvent;

	Settings mSettings;
};

