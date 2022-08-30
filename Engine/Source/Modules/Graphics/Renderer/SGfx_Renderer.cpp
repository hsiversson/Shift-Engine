#include "SGfx_Renderer.h"

#include "Graphics/Lighting/Shadows/SGfx_ShadowSystem.h"
#include "Graphics/Lighting/Raytracing/SGfx_Raytracing.h"
#include "Graphics/Lighting/SGfx_LightCulling.h"
#include "Graphics/Lighting/SGfx_ReflectionProbe.h"
#include "Graphics/Lighting/SGfx_AmbientOcclusion.h"
#include "Graphics/View/SGfx_View.h"
#include "Graphics/Material/SGfx_MaterialCache.h"
#include "Graphics/Misc/SGfx_DefaultTextures.h"
#include "Graphics/PostEffects/SGfx_PostEffects.h"
#include "SGfx_DrawInfo.h"
#include "SGfx_PrimitiveRenderer.h"

SC_CVAR(bool, gEnableTemporalAA, "Renderer.TAA.Enable", true);

SGfx_Renderer::SGfx_Renderer()
	: mEnvironment(nullptr)
	, mLatestTaskEvent(nullptr)
{

}

SGfx_Renderer::~SGfx_Renderer()
{

}

bool SGfx_Renderer::Init(SGfx_Environment* aEnvironment)
{
	if (!SGfx_DefaultTextures::Init())
		return false;

	const SC_IntVector2& backbufferResolution = SR_RenderDevice::gInstance->GetSwapChain()->GetProperties().mSize;

	SR_TextureResourceProperties depthStencilResourceProps;
	depthStencilResourceProps.mSize = SC_IntVector(backbufferResolution);
	depthStencilResourceProps.mFormat = SR_Format::D32_FLOAT;
	depthStencilResourceProps.mNumMips = 1;
	depthStencilResourceProps.mType = SR_ResourceType::Texture2D;
	depthStencilResourceProps.mAllowDepthStencil = true;
	depthStencilResourceProps.mAllowRenderTarget = false;
	depthStencilResourceProps.mAllowUnorderedAccess = false;

	SC_Ref<SR_TextureResource> depthStencilResource = SR_RenderDevice::gInstance->CreateTextureResource(depthStencilResourceProps);

	SR_DepthStencilProperties dsvProperties(SR_Format::D32_FLOAT);
	mDepthStencil = SR_RenderDevice::gInstance->CreateDepthStencil(dsvProperties, depthStencilResource);

	SR_TextureProperties dsvSRVProperties(SR_Format::D32_FLOAT);
	mDepthStencilSRV = SR_RenderDevice::gInstance->CreateTexture(dsvSRVProperties, depthStencilResource);

	mSceneColor.Init(SC_IntVector(backbufferResolution), SR_Format::RG11B10_FLOAT, true, true, "SceneColor");
	mSceneColor2.Init(SC_IntVector(backbufferResolution), SR_Format::RG11B10_FLOAT, true, true, "SceneColor2");
	mScreenColor.Init(SC_IntVector(backbufferResolution), SR_Format::RGBA8_UNORM, true, true, "ScreenColor");
	mDebugTarget.Init(SC_IntVector(backbufferResolution), SR_Format::RGBA8_UNORM, true, true, "DebugTarget");
	mHistoryBuffer.Init(SC_IntVector(backbufferResolution), SR_Format::RG11B10_FLOAT, true, true, "HistoryBuffer");
	mMotionVectors.Init(SC_IntVector(backbufferResolution), SR_Format::RG16_FLOAT, true, true, "MotionVectors");

	mGBuffer_Color.Init(SC_IntVector(backbufferResolution), SR_Format::RGBA8_UNORM, true, false, "GBufferColor");
	mGBuffer_Normals.Init(SC_IntVector(backbufferResolution), SR_Format::RGB10A2_UNORM, true, false, "GBufferNormals");
	mGBuffer_Material_RMAS.Init(SC_IntVector(backbufferResolution), SR_Format::RGBA8_UNORM, true, false, "GBufferMaterialRMAS");

	SR_ShaderCompileArgs compileArgs;
	compileArgs.mEntryPoint = "Main";
	compileArgs.mType = SR_ShaderType::Compute;
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/PostEffect_Tonemap.ssf";

	SR_ShaderStateProperties tonemapShaderProps;
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, tonemapShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &tonemapShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;
	mTonemapShader = SR_RenderDevice::gInstance->CreateShaderState(tonemapShaderProps);

	compileArgs.mEntryPoint = "TAAResolve";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/TemporalAA.ssf";
	compileArgs.mType = SR_ShaderType::Compute;

	SR_ShaderStateProperties taaShaderProps;
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, taaShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &taaShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;
	mTAAResolveShader = SR_RenderDevice::gInstance->CreateShaderState(taaShaderProps);

	SR_ShaderStateProperties copyShaderProps;
	compileArgs.mEntryPoint = "MainVS";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/PostEffect_Copy.ssf";
	compileArgs.mType = SR_ShaderType::Vertex;
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, copyShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Vertex)], &copyShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Vertex)]))
		return false;

	compileArgs.mEntryPoint = "MainPS";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/PostEffect_Copy.ssf";
	compileArgs.mType = SR_ShaderType::Pixel; 
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, copyShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Pixel)], &copyShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Pixel)]))
		return false;

	copyShaderProps.mRTVFormats.mNumColorFormats = 1;
	copyShaderProps.mRTVFormats.mColorFormats[0] = SR_RenderDevice::gInstance->GetSwapChain()->GetRenderTarget()->GetProperties().mFormat;
	copyShaderProps.mPrimitiveTopology = SR_PrimitiveTopology::TriangleList;
	mCopyShader = SR_RenderDevice::gInstance->CreateShaderState(copyShaderProps);

	SR_ShaderStateProperties lightingShaderProps;
	compileArgs.mEntryPoint = "Main";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/ComputeLight.ssf";
	compileArgs.mType = SR_ShaderType::Compute; 
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, lightingShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &lightingShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;
	mRenderLightingShader = SR_RenderDevice::gInstance->CreateShaderState(lightingShaderProps);

	mLightCulling = SC_MakeUnique<SGfx_LightCulling>();
	if (!mLightCulling->Init())
		return false;

	mShadowMapSystem = SC_MakeUnique<SGfx_ShadowSystem>();
	SGfx_CascadedShadowMap::Settings csmSettings;
	mShadowMapSystem->GetCSM()->Init(csmSettings);

	mRaytracingSystem = SC_MakeUnique<SGfx_Raytracing>();
	if (!mRaytracingSystem->Init())
		return false;

	mReflectionProbe = SC_MakeRef<SGfx_ReflectionProbe>();

	mAmbientOcclusion = SC_MakeUnique<SGfx_AmbientOcclusion>();
	if (!mAmbientOcclusion->Init())
		return false;

	mPostEffects = SC_MakeUnique<SGfx_PostEffects>();
	if (!mPostEffects->Init())
		return false;

	mDebugRenderer = SC_MakeUnique<SGfx_PrimitiveRenderer>();
	if (!mDebugRenderer->Init())
		return false;

	mEnvironment = aEnvironment;
	return true;
}

void SGfx_Renderer::RenderView(SGfx_View* aView)
{
	SGfx_ViewData& prepareData = aView->GetPrepareData();
	//SC_PROFILER_FUNCTION();

	aView->StartRender();

	//UpdateViewSettings();

#if SR_ENABLE_RAYTRACING
	prepareData.mBuildRaytracingSceneEvent = SubmitGraphicsTask(std::bind(&SGfx_Renderer::ComputeRaytracingScene, this, SC_Placeholder::Arg1), aView); // needs to be posted before PreRenderUpdates to make sure RaytracingScene descriptor is ready
#endif

	prepareData.mPreRenderUpdatesEvent = SubmitGraphicsTask(std::bind(&SGfx_Renderer::PreRenderUpdates, this, SC_Placeholder::Arg1), aView);

	prepareData.mPrePassEvent = SubmitGraphicsTask(std::bind(&SGfx_Renderer::RenderPrePass, this, SC_Placeholder::Arg1), aView);
	prepareData.mLightCullingEvent = SubmitGraphicsTask(std::bind(&SGfx_Renderer::ComputeLightCulling, this, SC_Placeholder::Arg1), aView);

	prepareData.mRenderGBufferEvent = SubmitGraphicsTask(std::bind(&SGfx_Renderer::RenderGBuffer, this, SC_Placeholder::Arg1), aView);
	prepareData.mAmbientOcclusionEvent = SubmitGraphicsTask(std::bind(&SGfx_Renderer::ComputeAmbientOcclusion, this, SC_Placeholder::Arg1), aView);
	prepareData.mRenderGIEvent = SubmitGraphicsTask(std::bind(&SGfx_Renderer::RenderGI, this, SC_Placeholder::Arg1), aView);

	prepareData.mRenderLightingEvent = SubmitGraphicsTask(std::bind(&SGfx_Renderer::RenderLighting, this, SC_Placeholder::Arg1), aView);
	//SubmitGraphicsTask(std::bind(&SGfx_Renderer::RenderShadows, this), prepareData.mShadowsEvent);

	prepareData.mRenderOpaqueEvent = SubmitGraphicsTask(std::bind(&SGfx_Renderer::RenderForward, this, SC_Placeholder::Arg1), aView);
	prepareData.mRenderDebugObjectsEvent = SubmitGraphicsTask(std::bind(&SGfx_Renderer::RenderDebugObjects, this, SC_Placeholder::Arg1), aView);
	prepareData.mPostEffectsEvent = SubmitGraphicsTask(std::bind(&SGfx_Renderer::ComputePostEffects, this, SC_Placeholder::Arg1), aView);

	//for (SC_Event* taskEvent : mSubmittedTaskEvents)
	//	taskEvent->Wait();
	//
	//mCurrentView->EndPrepare();
	aView->EndRender();

	mSubmittedTaskEvents.RemoveAll();

	SC_PROFILER_EVENT_END();
}

static float Halton(uint32 i, uint32 b)
{
	float f = 1.0f;
	float r = 0.0f;

	while (i > 0)
	{
		f /= static_cast<float>(b);
		r = r + f * static_cast<float>(i % b);
		i = static_cast<uint32>(SC_Math::Floor(static_cast<float>(i) / static_cast<float>(b)));
	}

	return r;
}

SC_Vector2 SGfx_Renderer::GetJitter(const SC_IntVector2& aTargetResolution) const
{
	static constexpr uint32 gSampleCount = 8;

	uint32 jitterIndex = SC_Time::gFrameCounter % gSampleCount;
	float haltonX = 2.0f * Halton(jitterIndex + 1, 2) - 1.0f;
	float haltonY = 2.0f * Halton(jitterIndex + 1, 3) - 1.0f;
	float jitterX = (haltonX / aTargetResolution.x);
	float jitterY = (haltonY / aTargetResolution.y);

	return SC_Vector2(jitterX, jitterY);
}

SGfx_LightCulling* SGfx_Renderer::GetLightCulling() const
{
	return mLightCulling.get();
}

SGfx_ShadowSystem* SGfx_Renderer::GetShadowMapSystem() const
{
	return mShadowMapSystem.get();
}

SGfx_AmbientOcclusion* SGfx_Renderer::GetAmbientOcclusion() const
{
	return mAmbientOcclusion.get();
}

SGfx_DDGI* SGfx_Renderer::GetRTGI() const
{
	if (mRaytracingSystem)
		return mRaytracingSystem->GetDDGI();

	return nullptr;
}

const SC_Ref<SR_Texture>& SGfx_Renderer::GetScreenColor() const
{
	return mScreenColor.mTexture;
}

SR_Texture* SGfx_Renderer::GetAoTex() const
{
	return mAmbientOcclusion->GetTexture();
}

SGfx_Renderer::Settings& SGfx_Renderer::GetSettings()
{
	return mSettings;
}

const SGfx_Renderer::Settings& SGfx_Renderer::GetSettings() const
{
	return mSettings;
}

void SGfx_Renderer::OnChanged(const SC_FilePath& aPath, const ChangeReason& /*aReason*/)
{
	SC_LOG("File Changed: {}", aPath.GetStr());

	SR_ShaderCompileArgs compileArgs;
	compileArgs.mEntryPoint = "Main";
	compileArgs.mShaderFile = aPath;
	compileArgs.mType = SR_ShaderType::Compute;

	SR_ShaderStateProperties tonemapShaderProps;
	if (SR_RenderDevice::gInstance->CompileShader(compileArgs, tonemapShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &tonemapShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
	{
		mTonemapShader = SR_RenderDevice::gInstance->CreateShaderState(tonemapShaderProps);
	}

}

SC_Ref<SR_TaskEvent> SGfx_Renderer::SubmitGraphicsTask(SGfx_ViewTaskFunctionSignature aTask, SGfx_View* aView)
{
	SC_Ref<SR_TaskEvent> taskEvent = SR_RenderDevice::gInstance->PostGraphicsTask([aTask, aView]() { aTask(aView); });
	mSubmittedTaskEvents.Add(&taskEvent->mCPUEvent);
	mLatestTaskEvent = &taskEvent->mCPUEvent;
	return taskEvent;
}

SC_Ref<SR_TaskEvent> SGfx_Renderer::SubmitComputeTask(SGfx_ViewTaskFunctionSignature aTask, SGfx_View* aView)
{
	SC_Ref<SR_TaskEvent> taskEvent = SR_RenderDevice::gInstance->PostComputeTask([aTask, aView]() { aTask(aView); });
	mSubmittedTaskEvents.Add(&taskEvent->mCPUEvent);
	mLatestTaskEvent = &taskEvent->mCPUEvent;
	return taskEvent;
}

SC_Ref<SR_TaskEvent> SGfx_Renderer::SubmitCopyTask(SGfx_ViewTaskFunctionSignature aTask, SGfx_View* aView)
{
	SC_Ref<SR_TaskEvent> taskEvent = SR_RenderDevice::gInstance->PostCopyTask([aTask, aView]() { aTask(aView); });
	mSubmittedTaskEvents.Add(&taskEvent->mCPUEvent);
	mLatestTaskEvent = &taskEvent->mCPUEvent;
	return taskEvent;
}

void SGfx_Renderer::PreRenderUpdates(SGfx_View* aView)
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	SGfx_ViewData& renderData = aView->GetMutableRenderData();
	aView->WaitForPrepareTask(renderData.mPrepareCullMeshesEvent);
	aView->WaitForPrepareTask(renderData.mPrepareCullLightsEvent);
#if SR_ENABLE_RAYTRACING
	cmdList->WaitFor(renderData.mBuildRaytracingSceneEvent);
#endif

	SC_PROFILER_FUNCTION();

	mLightCulling->Prepare(renderData);

	mRaytracingSystem->GetDDGI()->GetConstants(renderData);

	SGfx_MaterialGPUDataBuffer& materialGpuBuffer = SGfx_MaterialGPUDataBuffer::Get();
	materialGpuBuffer.UpdateBuffer();
	renderData.mSceneConstants.mMaterialInfoBufferIndex = SGfx_MaterialGPUDataBuffer::Get().GetBufferDescriptorIndex();

	SR_Buffer* instanceDataBuffer = renderData.mInstanceData->GetBuffer();
	renderData.mSceneConstants.mInstanceDataBufferIndex = instanceDataBuffer->GetDescriptorHeapIndex();

	SR_Buffer* rtInstanceDataBuffer = renderData.mRaytracingInstanceData->GetBuffer();
	renderData.mSceneConstants.mRaytracingInstanceDataBufferIndex = rtInstanceDataBuffer->GetDescriptorHeapIndex();

	SR_BufferResourceProperties cbDesc;
	cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
	cbDesc.mElementCount = 1;
	cbDesc.mElementSize = sizeof(SGfx_SceneConstants);
	mViewConstantsBuffer = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc, &renderData.mSceneConstants);

	cmdList->SetRootConstantBuffer(mViewConstantsBuffer, 1);
	mEnvironment->ComputeSkyAtmosphereLUTs(cmdList);
}

#if SR_ENABLE_RAYTRACING

void SGfx_Renderer::ComputeRaytracingScene(SGfx_View* aView)
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	SGfx_ViewData& renderData = aView->GetMutableRenderData();
	aView->WaitForPrepareTask(renderData.mPrepareCullMeshesEvent);

	//if (renderData.mRaytracingInstances.IsEmpty())
	//	return;

	SC_PROFILER_FUNCTION();
	cmdList->BeginEvent("Build Raytracing Scene");

	mRaytracingScene = cmdList->BuildRaytracingBuffer(renderData.mRaytracingInstances, nullptr);
	renderData.mSceneConstants.mRaytracingSceneDescriptorIndex = mRaytracingScene->GetDescriptorHeapIndex();

	cmdList->EndEvent();
}
#endif

void SGfx_Renderer::RenderShadows(SGfx_View* aView)
{
	SC_PROFILER_FUNCTION();
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	mShadowMapSystem->GetCSM()->Generate(cmdList, aView->GetRenderData());
}

void SGfx_Renderer::RenderPrePass(SGfx_View* aView)
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	const SGfx_ViewData& renderData = aView->GetRenderData();

	aView->WaitForPrepareTask(renderData.mPrepareCullMeshesEvent);
	cmdList->WaitFor(renderData.mPreRenderUpdatesEvent);

	SC_PROFILER_FUNCTION();
	cmdList->BeginEvent("Render PrePass");
	SC_Array<SC_Pair<uint32, SR_TrackedResource*>> transitions;
	transitions.Add(SC_Pair(SR_ResourceState_DepthWrite, mDepthStencil->GetResource()));
	transitions.Add(SC_Pair(SR_ResourceState_RenderTarget, mMotionVectors.mResource));
	cmdList->TransitionBarrier(transitions);

	cmdList->ClearRenderTarget(mMotionVectors.mRenderTarget, SC_Vector4(0));
	cmdList->ClearDepthStencil(mDepthStencil);
	cmdList->SetRenderTargets(0, nullptr, mDepthStencil);

	SR_Rect rect = 
	{
		0,
		0,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.x,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.y
	};

	cmdList->SetViewport(rect);
	cmdList->SetScissorRect(rect);

	cmdList->SetRootConstantBuffer(mViewConstantsBuffer, 1);

	cmdList->SetRenderTargets(0, nullptr, mDepthStencil);
	renderData.mDepthQueue.Render(cmdList);

	cmdList->SetRenderTarget(mMotionVectors.mRenderTarget, mDepthStencil);
	renderData.mDepthQueue_MotionVectors.Render(cmdList);

	transitions.RemoveAll();
	transitions.Add(SC_Pair(SR_ResourceState_DepthRead, mDepthStencil->GetResource()));
	transitions.Add(SC_Pair(SR_ResourceState_Read, mMotionVectors.mResource));
	cmdList->TransitionBarrier(transitions);
	cmdList->EndEvent(); // Render PrePass
}

void SGfx_Renderer::RenderGBuffer(SGfx_View* aView)
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	SGfx_ViewData& renderData = aView->GetMutableRenderData();

	cmdList->WaitFor(renderData.mPrePassEvent);

	SC_PROFILER_FUNCTION();
	cmdList->BeginEvent("Render GBuffer");

	SC_Array<SR_RenderTarget*> gbufferTargets;
	gbufferTargets.Add(mGBuffer_Color.mRenderTarget);
	gbufferTargets.Add(mGBuffer_Normals.mRenderTarget);
	gbufferTargets.Add(mGBuffer_Material_RMAS.mRenderTarget);

	SC_Array<SC_Pair<uint32, SR_TrackedResource*>> barriers;
	barriers.Add(SC_Pair(SR_ResourceState_RenderTarget, mGBuffer_Color.mResource));
	barriers.Add(SC_Pair(SR_ResourceState_RenderTarget, mGBuffer_Normals.mResource));
	barriers.Add(SC_Pair(SR_ResourceState_RenderTarget, mGBuffer_Material_RMAS.mResource));
	cmdList->TransitionBarrier(barriers);

	cmdList->ClearRenderTargets(gbufferTargets.Count(), gbufferTargets.GetBuffer(), SC_Vector4(0.f, 0.f, 0.f, 0.f));
	cmdList->SetRenderTargets(gbufferTargets.Count(), gbufferTargets.GetBuffer(), mDepthStencil);

	SR_Rect rect =
	{
		0,
		0,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.x,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.y
	};
	cmdList->SetViewport(rect);
	cmdList->SetScissorRect(rect);

	cmdList->SetRootConstantBuffer(mViewConstantsBuffer, 1);

	renderData.mOpaqueQueue.Render(cmdList);

	barriers.RemoveAll();
	barriers.Add(SC_Pair(SR_ResourceState_Read, mGBuffer_Color.mResource));
	barriers.Add(SC_Pair(SR_ResourceState_Read, mGBuffer_Normals.mResource));
	barriers.Add(SC_Pair(SR_ResourceState_Read, mGBuffer_Material_RMAS.mResource));
	cmdList->TransitionBarrier(barriers);

	renderData.mSceneConstants.mGBufferConstants.mDepthDescriptorIndex = mDepthStencilSRV->GetDescriptorHeapIndex();
	renderData.mSceneConstants.mGBufferConstants.mColorDescriptorIndex = mGBuffer_Color.mTexture->GetDescriptorHeapIndex();
	renderData.mSceneConstants.mGBufferConstants.mNormalsDescriptorIndex = mGBuffer_Normals.mTexture->GetDescriptorHeapIndex();
	renderData.mSceneConstants.mGBufferConstants.mMaterialRMASDescriptorIndex = mGBuffer_Material_RMAS.mTexture->GetDescriptorHeapIndex();
	cmdList->UpdateBuffer(mViewConstantsBuffer, 0, &renderData.mSceneConstants, sizeof(renderData.mSceneConstants));

	cmdList->EndEvent();

}

void SGfx_Renderer::ComputeLightCulling(SGfx_View* aView)
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	const SGfx_ViewData& renderData = aView->GetRenderData();

	cmdList->WaitFor(renderData.mPrePassEvent);

	SC_PROFILER_FUNCTION();
	cmdList->SetRootConstantBuffer(mViewConstantsBuffer, 1);
	mLightCulling->CullLights(cmdList, renderData, mDepthStencilSRV);
}

void SGfx_Renderer::ComputeAmbientOcclusion(SGfx_View* aView)
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	SGfx_ViewData& renderData = aView->GetMutableRenderData();

#if SR_ENABLE_RAYTRACING
	if (renderData.mRaytracingInstances.IsEmpty())
		return;
#endif

	cmdList->WaitFor(renderData.mPrePassEvent);

	SC_PROFILER_FUNCTION();
	cmdList->BeginEvent("Ambient Occlusion");

	cmdList->SetRootConstantBuffer(mViewConstantsBuffer, 1);
	mAmbientOcclusion->Render(cmdList, mDepthStencilSRV, renderData);
	renderData.mSceneConstants.mAmbientOcclusionDescriptorIndex = mAmbientOcclusion->GetTexture()->GetDescriptorHeapIndex();

	cmdList->UpdateBuffer(mViewConstantsBuffer, 0, &renderData.mSceneConstants, sizeof(renderData.mSceneConstants));

	cmdList->EndEvent(); // Ambient Occlusion
}

void SGfx_Renderer::RenderGI(SGfx_View* aView)
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	const SGfx_ViewData& renderData = aView->GetRenderData();
	cmdList->WaitFor(renderData.mAmbientOcclusionEvent);
	cmdList->WaitFor(renderData.mLightCullingEvent);

	SC_PROFILER_FUNCTION();

	cmdList->SetRootConstantBuffer(mViewConstantsBuffer, 1);
	mRaytracingSystem->GetDDGI()->RenderDiffuse(aView);
	mRaytracingSystem->GetDDGI()->RenderSpecular(aView);
}

void SGfx_Renderer::RenderLighting(SGfx_View* aView)
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	const SGfx_ViewData& renderData = aView->GetRenderData();
	cmdList->WaitFor(renderData.mRenderGBufferEvent);
	cmdList->WaitFor(renderData.mLightCullingEvent);

	SC_PROFILER_FUNCTION();

	cmdList->BeginEvent("Render Lighting");
	cmdList->TransitionBarrier(SR_ResourceState_UnorderedAccess, mSceneColor.mResource);

	const SR_Rect screenRect =
	{
		0,
		0,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.x,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.y
	};

	struct Constants
	{
		SC_Vector4 mTargetResolutionAndRcp;
		uint32 mOutputDescriptorIndex;
		uint32 _unused[3];
	} constants;

	constants.mTargetResolutionAndRcp = SC_Vector4((float)mSceneColor.mResource->GetProperties().mSize.x, (float)mSceneColor.mResource->GetProperties().mSize.y, 1.0f / mSceneColor.mResource->GetProperties().mSize.x, 1.0f / mSceneColor.mResource->GetProperties().mSize.y);
	constants.mOutputDescriptorIndex = mSceneColor.mTextureRW->GetDescriptorHeapIndex();

	uint64 cbOffset = 0;
	SR_BufferResource* cb = cmdList->GetBufferResource(cbOffset, SR_BufferBindFlag_ConstantBuffer, sizeof(constants), &constants, 1);

	cmdList->SetRootConstantBuffer(cb, cbOffset, 0);

	cmdList->SetRootConstantBuffer(mViewConstantsBuffer, 1);
	cmdList->Dispatch(mRenderLightingShader, SC_IntVector(screenRect.mRight, screenRect.mBottom, 1));
	cmdList->EndEvent();
}

void SGfx_Renderer::RenderForward(SGfx_View* aView)
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	const SGfx_ViewData& renderData = aView->GetRenderData();

	cmdList->WaitFor(renderData.mRenderLightingEvent);

	SC_PROFILER_FUNCTION();
	cmdList->BeginEvent("Render Opaque");

	SC_Array<SC_Pair<uint32, SR_TrackedResource*>> barriers;
	barriers.Add(SC_Pair(SR_ResourceState_RenderTarget, mSceneColor.mResource));
	cmdList->TransitionBarrier(barriers);
	cmdList->SetRenderTarget(mSceneColor.mRenderTarget, mDepthStencil);

	SR_Rect rect =
	{
		0,
		0,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.x,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.y
	};
	cmdList->SetViewport(rect);
	cmdList->SetScissorRect(rect);

	cmdList->SetRootConstantBuffer(mViewConstantsBuffer, 1);

	//renderData.mOpaqueQueue.Render(cmdList);
	//renderData.mTransparentQueue.Render(cmdList);

	if (renderData.mSky)
		renderData.mSky->Render(cmdList);

	if (mRaytracingSystem->GetDDGI()->mProbeGridProperties.mVisualizeProbes)
	{
		cmdList->TransitionBarrier(SR_ResourceState_DepthWrite, mDepthStencil->GetResource());
		mRaytracingSystem->GetDDGI()->DebugRenderProbes(aView);
		cmdList->TransitionBarrier(SR_ResourceState_DepthRead, mDepthStencil->GetResource());
	}
	cmdList->EndEvent(); // Render Opaque
}

void SGfx_Renderer::RenderDebugObjects(SGfx_View* aView)
{
	SC_PROFILER_FUNCTION();
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	const SGfx_ViewData& renderData = aView->GetRenderData();
	cmdList->BeginEvent("DebugDraw");

	cmdList->TransitionBarrier(SR_ResourceState_RenderTarget, mDebugTarget.mResource);
	cmdList->ClearRenderTarget(mDebugTarget.mRenderTarget, SC_Vector4(0.f, 0.f, 0.f, 0.f));

	SR_Rect rect =
	{
		0,
		0,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.x,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.y
	};
	cmdList->SetViewport(rect);
	cmdList->SetScissorRect(rect);

	cmdList->SetRenderTarget(mDebugTarget.mRenderTarget, mDepthStencil);
	cmdList->SetRootConstantBuffer(mViewConstantsBuffer, 1);

	mDebugRenderer->SetDrawGrid(mSettings.mDrawGridHelper);
	mDebugRenderer->Render(cmdList, renderData);

	cmdList->TransitionBarrier(SR_ResourceState_Read, mDebugTarget.mResource);
	cmdList->EndEvent();
}

void SGfx_Renderer::ComputePostEffects(SGfx_View* aView)
{
	SC_PROFILER_FUNCTION();
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	const SGfx_ViewData& renderData = aView->GetRenderData();
	const SR_Rect screenRect =
	{
		0,
		0,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.x,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.y
	};

	cmdList->SetRootConstantBuffer(mViewConstantsBuffer, 1);

	if (mSettings.mEnableTemporalAA && gEnableTemporalAA)
	{
		cmdList->BeginEvent("TAA");
		cmdList->TransitionBarrier(SR_ResourceState_UnorderedAccess, mSceneColor2.mResource);
		struct TAAConstants
		{
			SC_Vector4 mTargetResolutionAndRcp;

			uint32 mTextureDescriptorIndex;
			uint32 mHistoryTextureDescriptorIndex;
			uint32 mMotionVectorTextureDescriptorIndex;
			uint32 mDepthStencilTextureDescriptorIndex;

			uint32 mOutputTextureDescriptorIndex;
			uint32 _unused[3];
		} taaConstants = {};
		taaConstants.mTextureDescriptorIndex = mSceneColor.mTexture->GetDescriptorHeapIndex();
		taaConstants.mHistoryTextureDescriptorIndex = mHistoryBuffer.mTexture->GetDescriptorHeapIndex();
		taaConstants.mMotionVectorTextureDescriptorIndex = mMotionVectors.mTexture->GetDescriptorHeapIndex();
		taaConstants.mDepthStencilTextureDescriptorIndex = mDepthStencilSRV->GetDescriptorHeapIndex();
		taaConstants.mOutputTextureDescriptorIndex = mSceneColor2.mTextureRW->GetDescriptorHeapIndex();
		taaConstants.mTargetResolutionAndRcp = SC_Vector4((float)mSceneColor2.mResource->GetProperties().mSize.x, (float)mSceneColor2.mResource->GetProperties().mSize.y, 1.0f / mSceneColor2.mResource->GetProperties().mSize.x, 1.0f / mSceneColor2.mResource->GetProperties().mSize.y);

		uint64 cbOffset = 0;
		SR_BufferResource* cb = cmdList->GetBufferResource(cbOffset, SR_BufferBindFlag_ConstantBuffer, sizeof(taaConstants), &taaConstants, 1);

		cmdList->SetRootConstantBuffer(cb, cbOffset, 0);
		cmdList->Dispatch(mTAAResolveShader, SC_IntVector(screenRect.mRight, screenRect.mBottom, 1));

		SC_Array<SC_Pair<uint32, SR_TrackedResource*>> barriers;
		barriers.Add(SC_Pair(SR_ResourceState_CopySrc, mSceneColor2.mResource));
		barriers.Add(SC_Pair(SR_ResourceState_CopyDst, mHistoryBuffer.mResource));
		cmdList->TransitionBarrier(barriers);

		cmdList->CopyResource(mHistoryBuffer.mResource, mSceneColor2.mResource);

		barriers.RemoveAll();
		barriers.Add(SC_Pair(SR_ResourceState_Read, mSceneColor2.mResource));
		barriers.Add(SC_Pair(SR_ResourceState_Read, mHistoryBuffer.mResource));
		cmdList->TransitionBarrier(barriers);

		cmdList->EndEvent(); // TAA
	}
	// Tonemap and Composite
	cmdList->BeginEvent("Compute Post Effects");

	cmdList->BeginEvent("Bloom");
	mPostEffects->Render(aView, (mSettings.mEnableTemporalAA) ? mSceneColor2.mTexture : mSceneColor.mTexture);
	cmdList->EndEvent();

	cmdList->BeginEvent("Tonemap");
	cmdList->TransitionBarrier(SR_ResourceState_UnorderedAccess, mScreenColor.mResource);

	struct DispatchInfo
	{
		SC_Vector4 mTargetResolutionAndRcp;
		uint32 mSceneColorDescriptorIndex;
		uint32 mBloomTextureDescriptorIndex;
		uint32 mDebugTargetTextureDescriptorIndex;
		uint32 mOutputTextureDescriptorIndex;
	} dispatchInfo;
	const SC_IntVector2 targetSize = mScreenColor.mResource->GetProperties().mSize.XY();
	dispatchInfo.mTargetResolutionAndRcp = SC_Vector4(targetSize, SC_Vector2(1.0f / targetSize.x, 1.0f / targetSize.y));
	dispatchInfo.mSceneColorDescriptorIndex = (mSettings.mEnableTemporalAA) ? mSceneColor2.mTexture->GetDescriptorHeapIndex() : mSceneColor.mTexture->GetDescriptorHeapIndex();
	dispatchInfo.mBloomTextureDescriptorIndex = mPostEffects->GetBloomTexture()->GetDescriptorHeapIndex();
	dispatchInfo.mDebugTargetTextureDescriptorIndex = mDebugTarget.mTexture->GetDescriptorHeapIndex();
	dispatchInfo.mOutputTextureDescriptorIndex = mScreenColor.mTextureRW->GetDescriptorHeapIndex();

	uint64 cbOffset = 0;
	SR_BufferResource* cb = cmdList->GetBufferResource(cbOffset, SR_BufferBindFlag_ConstantBuffer, sizeof(dispatchInfo), &dispatchInfo, 1);

	cmdList->SetRootConstantBuffer(cb, cbOffset, 0);
	cmdList->Dispatch(mTonemapShader, SC_IntVector(screenRect.mRight, screenRect.mBottom, 1));

	cmdList->TransitionBarrier(SR_ResourceState_Read, mScreenColor.mResource);
	cmdList->EndEvent(); // tonemap

	cmdList->EndEvent(); // Compute Post Effects
}

