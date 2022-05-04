#pragma once
#include "SGfx_Camera.h"
#include "Graphics/Lighting/SGfx_Light.h"
#include "Graphics/Lighting/Shadows/SGfx_ShadowConstants.h"
#include "Graphics/Lighting/SGfx_LightCulling.h"
#include "Graphics/Environment/SGfx_Environment.h"
#include "Graphics/View/SGfx_RenderQueue.h"
#include "RenderCore/RenderTasks/SR_TaskEvent.h"
#include "Platform/Async/SC_Future.h"

class SGfx_MaterialInstance;
class SR_Buffer;
class SR_BufferResource;
class SR_ShaderState;
class SGfx_View;

struct SGfx_RenderObject
{
	SC_Matrix mTransform;
	SC_Matrix mPrevTransform;
	float mSortDistance;

	SR_BufferResource* mVertexBufferResource;
	SR_BufferResource* mIndexBufferResource;

#if ENABLE_MESH_SHADERS
	SR_Buffer* mVertexBuffer;
	SR_Buffer* mMeshletBuffer;
	SR_Buffer* mVertexIndexBuffer;
	SR_Buffer* mPrimitiveIndexBuffer;
#endif

	SR_ShaderState* mShader;
	uint32 mMaterialIndex;
	bool mOutputVelocity;
};

struct SGfx_LightRenderData
{
	SGfx_Light::LocalLightShaderData mGPUData;
	bool mCastShadow;
	SC_Array<SGfx_RenderObject> mVisibleShadowCasters;
};

struct alignas(16) SGfx_SceneConstants
{
	void Clear()
	{
		SC_ZeroMemory(this, sizeof(SGfx_SceneConstants));
	}

	SGfx_ViewConstants mViewConstants;
	SGfx_EnvironmentConstants mEnvironmentConstants;
	SGfx_ShadowConstants mShadowConstants;
	SGfx_LightCullingConstants mLightCullingConstants;

	uint32 mInstanceDataBufferIndex;
	uint32 mMaterialInfoBufferIndex;
	uint32 mRaytracingSceneDescriptorIndex;
	uint32 mFrameIndex;
	float mFrameTimeDelta;
	uint32 pad[3];
};

struct SGfx_ViewRenderSettings
{
	void Clear()
	{
		SC_ZeroMemory(this, sizeof(SGfx_ViewRenderSettings));
	}

	bool mRenderDepth;
	bool mRenderOpaque;
	bool mRenderTransparent;
};

struct SGfx_ViewData
{
public:
	SGfx_ViewData()
	{
		SC_ZeroMemory(this, sizeof(SGfx_ViewData));

		mBuildRaytracingSceneEvent = SC_MakeUnique<SR_TaskEvent>();
		mPreRenderUpdatesEvent = SC_MakeUnique<SR_TaskEvent>();
		mPrePassEvent = SC_MakeUnique<SR_TaskEvent>();
		mLightCullingEvent = SC_MakeUnique<SR_TaskEvent>();
		mShadowsEvent = SC_MakeUnique<SR_TaskEvent>();
		mAmbientOcclusionEvent = SC_MakeUnique<SR_TaskEvent>();
		mRenderOpaqueEvent = SC_MakeUnique<SR_TaskEvent>();
		mRenderDebugObjectsEvent = SC_MakeUnique<SR_TaskEvent>();
		mPostEffectsEvent = SC_MakeUnique<SR_TaskEvent>();
	}

	void Clear()
	{
		mSky = nullptr;

		mRenderSettings.Clear();
		mSceneConstants.Clear();

		_mDepthQueue.Clear();
		_mOpaqueQueue.Clear();
		_mTransparentQueue.Clear();

		mDepthQueue.RemoveAll();
		mOpaqueQueue.RemoveAll();
		mTranslucencyQueue.RemoveAll();
		mVisibleLights.RemoveAll();
		mCSMViews.RemoveAll();
#if ENABLE_RAYTRACING
		mRaytracingInstances.RemoveAll();
#endif

		mBuildRaytracingSceneEvent->Reset();
		mPreRenderUpdatesEvent->Reset();
		mPrePassEvent->Reset();
		mLightCullingEvent->Reset();
		mShadowsEvent->Reset();
		mAmbientOcclusionEvent->Reset();
		mRenderOpaqueEvent->Reset();
		mRenderDebugObjectsEvent->Reset();
		mPostEffectsEvent->Reset();
	}

	SGfx_Sky* mSky;

	SGfx_ViewRenderSettings mRenderSettings;
	SGfx_SceneConstants mSceneConstants;

	SGfx_RenderQueue_ByState _mDepthQueue;
	SGfx_RenderQueue_ByState _mOpaqueQueue;
	SGfx_RenderQueue_FarFirst _mTransparentQueue;

	SC_Array<SGfx_RenderObject> mDepthQueue;		// Switch to a renderQueue
	SC_Array<SGfx_RenderObject> mOpaqueQueue;		// Switch to a renderQueue
	SC_Array<SGfx_RenderObject> mTranslucencyQueue; // Switch to a renderQueue

	SC_Array<SGfx_LightRenderData> mVisibleLights;

	SC_Array<SC_Ref<SGfx_View>> mCSMViews;

#if ENABLE_RAYTRACING
	SC_Array<SR_RaytracingInstanceData> mRaytracingInstances;
#endif

	SC_UniquePtr<SR_TaskEvent> mBuildRaytracingSceneEvent;
	SC_UniquePtr<SR_TaskEvent> mPreRenderUpdatesEvent;
	SC_UniquePtr<SR_TaskEvent> mPrePassEvent;
	SC_UniquePtr<SR_TaskEvent> mLightCullingEvent;
	SC_UniquePtr<SR_TaskEvent> mShadowsEvent;
	SC_UniquePtr<SR_TaskEvent> mAmbientOcclusionEvent;
	SC_UniquePtr<SR_TaskEvent> mRenderOpaqueEvent;
	SC_UniquePtr<SR_TaskEvent> mRenderDebugObjectsEvent;
	SC_UniquePtr<SR_TaskEvent> mPostEffectsEvent;

};