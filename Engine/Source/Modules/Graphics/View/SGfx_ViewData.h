#pragma once
#include "SGfx_Camera.h"
#include "Graphics/Lighting/SGfx_Light.h"
#include "Graphics/Lighting/Shadows/SGfx_ShadowConstants.h"
#include "Graphics/Lighting/SGfx_LightCulling.h"
#include "Graphics/Environment/SGfx_Environment.h"
#include "Graphics/View/SGfx_RenderQueue.h"
#include "Graphics/Mesh/SGfx_InstanceData.h"
#include "RenderCore/RenderTasks/SR_TaskEvent.h"
#include "RenderCore/Interface/SR_RaytracingStructs.h"
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

#if SR_ENABLE_MESH_SHADERS
	SR_Buffer* mVertexBuffer;
	SR_Buffer* mMeshletBuffer;
	SR_Buffer* mVertexIndexBuffer;
	SR_Buffer* mPrimitiveIndexBuffer;
#endif

	SR_ShaderState* mShader;
	uint32 mMaterialIndex;
	uint32 mInstanceDataOffset;
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

	bool mRenderPrepass;
	bool mRenderOpaque;
	bool mRenderTranslucent;
	bool mRenderUI;
	bool mRenderPostEffects;

	bool mRenderMotionVectors;

	bool mRenderDecals;
	bool mRenderParticles;
	bool mRenderTerrain;
	bool mRenderWater;

	bool mRenderSky;
	bool mRenderSun;
	bool mRenderClouds;
	bool mRenderFog;
	bool mComputeScattering;

	bool mRenderDebugPrimitives;

	bool mEnableShadowMaps;
	bool mEnableCascadedShadowMaps;
	bool mEnableDynamicShadowCascades;
	bool mEnableFarShadows;

	bool mEnableTAA;
	bool mEnableRoughnessAA;

	bool mEnableRTAO;
	bool mEnableRTGI;
	bool mEnableRaytracedLocalShadows;
	bool mEnableRaytracedSunShadows;
	bool mEnableRaytracedReflections;
	bool mEnableRaytracedRefractions;
};

struct SGfx_ViewData
{
public:
	SGfx_ViewData()
	{
		SC_ZeroMemory(this, sizeof(SGfx_ViewData));
		mInstanceData = SC_MakeUnique<SGfx_InstanceData>();
	}

	void Clear()
	{
		SC_PROFILER_FUNCTION();
		mSky = nullptr;

		mViewRenderSettings.Clear();
		mSceneConstants.Clear();

		mDepthQueue.Clear();
		mDepthQueue_MotionVectors.Clear();
		mOpaqueQueue.Clear();
		mTransparentQueue.Clear();

		mVisibleLights.RemoveAll();
		mCSMViews.RemoveAll();
#if SR_ENABLE_RAYTRACING
		mRaytracingInstances.RemoveAll();
#endif

		mPrepareCullMeshesEvent = SC_Future<bool>();
		mPrepareCullLightsEvent = SC_Future<bool>();
		mPrepareLightCullingEvent = SC_Future<bool>();

		mBuildRaytracingSceneEvent.Reset();
		mPreRenderUpdatesEvent.Reset();
		mPrePassEvent.Reset();
		mLightCullingEvent.Reset();
		mShadowsEvent.Reset();
		mAmbientOcclusionEvent.Reset();
		mRenderOpaqueEvent.Reset();
		mRenderDebugObjectsEvent.Reset();
		mPostEffectsEvent.Reset();

		mInstanceData->Clear();

		mFrameIndex = 0;
		mDeltaTime = 0.0f;
		mElapsedTime = 0.0f;
	}

	SGfx_Sky* mSky;

	SGfx_ViewRenderSettings mViewRenderSettings;
	SGfx_SceneConstants mSceneConstants;

	SGfx_RenderQueue_ByState mDepthQueue;
	SGfx_RenderQueue_ByState mDepthQueue_MotionVectors;
	SGfx_RenderQueue_ByState mOpaqueQueue;
	SGfx_RenderQueue_FarFirst mTransparentQueue;

	SC_Array<SGfx_LightRenderData> mVisibleLights;

	SC_Array<SC_Ref<SGfx_View>> mCSMViews;

#if SR_ENABLE_RAYTRACING
	SC_Array<SR_RaytracingInstanceData> mRaytracingInstances;
#endif

	// PrepareEvents
	SC_Future<bool> mPrepareCullMeshesEvent;
	SC_Future<bool> mPrepareCullLightsEvent;
	SC_Future<bool> mPrepareLightCullingEvent;

	// RenderEvents
	SC_Ref<SR_TaskEvent> mBuildRaytracingSceneEvent;
	SC_Ref<SR_TaskEvent> mPreRenderUpdatesEvent;
	SC_Ref<SR_TaskEvent> mPrePassEvent;
	SC_Ref<SR_TaskEvent> mLightCullingEvent;
	SC_Ref<SR_TaskEvent> mShadowsEvent;
	SC_Ref<SR_TaskEvent> mAmbientOcclusionEvent;
	SC_Ref<SR_TaskEvent> mRenderOpaqueEvent;
	SC_Ref<SR_TaskEvent> mRenderDebugObjectsEvent;
	SC_Ref<SR_TaskEvent> mPostEffectsEvent;

	SC_UniquePtr<SGfx_InstanceData> mInstanceData;

	uint32 mFrameIndex;
	float mDeltaTime;
	float mElapsedTime;
};