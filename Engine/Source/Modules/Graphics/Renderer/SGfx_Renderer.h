#pragma once
#include "Graphics/Misc/SGfx_Surface.h"
#include "RenderCore/Defines/SR_Format.h"
#include "RenderCore/RenderTasks/SR_CommandQueueManager.h"

#include "InputOutput/File/SC_FileWatcher.h"

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
class SGfx_LightCulling;
class SGfx_ReflectionProbe;
class SGfx_AmbientOcclusion;
class SGfx_PostEffects;
class SGfx_PrimitiveRenderer;
class SGfx_Environment;
class SGfx_DDGI;

/**
 * 
 *		Default scene renderer.
 *		Renders views with a tiled forward path.
 * 
 */
using SGfx_ViewTaskFunctionSignature = std::function<void(SGfx_View* aView)>;
class SGfx_Renderer : public SC_FileWatcherListener
{
public:
	struct Settings
	{
		Settings()
			: mEnableTemporalAA(true)
			, mDrawGridHelper(false)
		{}

		bool mEnableTemporalAA;
		bool mDrawGridHelper;
	};

public:
	SGfx_Renderer();
	~SGfx_Renderer();

	bool Init(SGfx_Environment* aEnvironment);

	// Called from MainThread to kick off render tasks
	// This function returns after scheduling all render tasks.
	void RenderView(SGfx_View* aView);

	SC_Vector2 GetJitter(const SC_IntVector2& aTargetResolution) const;

	SGfx_LightCulling* GetLightCulling() const;
	SGfx_ShadowSystem* GetShadowMapSystem() const;
	SGfx_AmbientOcclusion* GetAmbientOcclusion() const;
	SGfx_DDGI* GetRTGI() const;

	const SC_Ref<SR_Texture>& GetScreenColor() const;
	SR_Texture* GetAoTex() const;

	Settings& GetSettings();
	const Settings& GetSettings() const;

protected:
	void OnChanged(const SC_FilePath& aPath, const ChangeReason& aReason) override;

private:
	SC_Ref<SR_TaskEvent> SubmitGraphicsTask(SGfx_ViewTaskFunctionSignature aTask, SGfx_View* aView);
	SC_Ref<SR_TaskEvent> SubmitComputeTask(SGfx_ViewTaskFunctionSignature aTask, SGfx_View* aView);
	SC_Ref<SR_TaskEvent> SubmitCopyTask(SGfx_ViewTaskFunctionSignature aTask, SGfx_View* aView);

	/////////////////////////
	// Render Tasks
	void PreRenderUpdates(SGfx_View* aView);

#if SR_ENABLE_RAYTRACING
	void ComputeRaytracingScene(SGfx_View* aView);
#endif

	void RenderShadows(SGfx_View* aView);
	void RenderPrePass(SGfx_View* aView); // Depth, Visibility Buffer

	void RenderGBuffer(SGfx_View* aView);

	void ComputeHierarchicalZ(SGfx_View* aView); // Generate HZB Chain
	void ComputeLightCulling(SGfx_View* aView);
	void ComputeAmbientOcclusion(SGfx_View* aView);

	void RenderGI(SGfx_View* aView);

	void RenderOpaque(SGfx_View* aView);

	void ComputeParticles(SGfx_View* aView);

	void RenderVolumetrics(SGfx_View* aView);
	void RenderTranslucency(SGfx_View* aView);

	void RenderDebugObjects(SGfx_View* aView);
	void RenderUI(SGfx_View* aView);

	void ComputePostEffects(SGfx_View* aView);
	/////////////////////////

	SC_UniquePtr<SGfx_LightCulling> mLightCulling;
	SC_UniquePtr<SGfx_ShadowSystem> mShadowMapSystem;
	SC_UniquePtr<SGfx_Raytracing> mRaytracingSystem; 
	SC_UniquePtr<SGfx_AmbientOcclusion> mAmbientOcclusion;
	SC_UniquePtr<SGfx_PostEffects> mPostEffects;
	SC_UniquePtr<SGfx_PrimitiveRenderer> mDebugRenderer;
	SGfx_Environment* mEnvironment;

	// Temp
	SC_Ref<SR_BufferResource> mViewConstantsBuffer;

	SC_Ref<SR_ShaderState> mTonemapShader;
	SC_Ref<SR_ShaderState> mTAAResolveShader;
	SC_Ref<SR_ShaderState> mCopyShader;
	SC_Ref<SR_DepthStencil> mDepthStencil;
	SC_Ref<SR_Texture> mDepthStencilSRV;
	SGfx_Surface mSceneColor;
	SGfx_Surface mSceneColor2;
	SGfx_Surface mHistoryBuffer;
	SGfx_Surface mScreenColor;
	SGfx_Surface mDebugTarget;
	SGfx_Surface mMotionVectors;
	SC_Ref<SR_Buffer> mRaytracingScene;
	//

	SC_Ref<SGfx_ReflectionProbe> mReflectionProbe;

	SC_Array<SC_Event*> mSubmittedTaskEvents;
	SC_Event* mLatestTaskEvent;

	Settings mSettings;
};

