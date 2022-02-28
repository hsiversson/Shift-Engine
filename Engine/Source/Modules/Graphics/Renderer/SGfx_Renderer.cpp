#include "SGfx_Renderer.h"

#include "Graphics/Lighting/Shadows/SGfx_ShadowSystem.h"
#include "Graphics/Lighting/Raytracing/SGfx_Raytracing.h"
#include "Graphics/Lighting/SGfx_LightCulling.h"
#include "Graphics/Lighting/SGfx_ReflectionProbe.h"
#include "Graphics/Lighting/SGfx_AmbientOcclusion.h"
#include "Graphics/Environment/SGfx_Sky.h"
#include "Graphics/View/SGfx_View.h"
#include "Graphics/Material/SGfx_MaterialCache.h"
#include "Graphics/Misc/SGfx_DefaultTextures.h"
#include "Graphics/PostEffects/SGfx_PostEffects.h"
#include "SGfx_DrawInfo.h"
#include "SGfx_DebugRenderer.h"

SC_CVAR(bool, gEnableTemporalAA, "Renderer.TAA.Enable", true);

SGfx_Renderer::SGfx_Renderer()
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

	SR_BufferResourceProperties cbDesc;
	cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
	cbDesc.mElementCount = 1;
	cbDesc.mElementSize = sizeof(SGfx_SceneConstants);
	mViewConstantsBuffer = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);

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

	SR_ShaderCompileArgs compileArgs;
	compileArgs.mEntryPoint = "Tonemap";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/PostEffect_Tonemap.ssf";
	compileArgs.mType = SR_ShaderType::Compute;

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

	mPostEffectCBuffers.Respace(4);

	mShadowMapSystem = SC_MakeUnique<SGfx_ShadowSystem>();
	SGfx_CascadedShadowMap::Settings csmSettings;
	mShadowMapSystem->GetCSM()->Init(csmSettings);

	mSky = SC_MakeUnique<SGfx_Sky>();
	if (!mSky->Init())
		return false;

	mReflectionProbe = SC_MakeRef<SGfx_ReflectionProbe>();

	mAmbientOcclusion = SC_MakeUnique<SGfx_AmbientOcclusion>();
	if (!mAmbientOcclusion->Init())
		return false;

	mPostEffects = SC_MakeUnique<SGfx_PostEffects>();
	if (!mPostEffects->Init())
		return false;

	mDebugRenderer = SC_MakeUnique<SGfx_DebugRenderer>();
	if (!mDebugRenderer->Init())
		return false;

	mEnvironment = aEnvironment;
	return true;
}

static SR_Fence lastFrameFence;
void SGfx_Renderer::RenderView(SGfx_View* aView)
{
	lastFrameFence.Wait();
	SGfx_ViewData& prepareData = aView->GetPrepareData();

	mCurrentView = aView;

#if ENABLE_RAYTRACING
	SubmitComputeTask(std::bind(&SGfx_Renderer::ComputeRaytracingScene, this), prepareData.mBuildRaytracingSceneEvent); // needs to be posted before PreRenderUpdates to make sure RaytracingScene descriptor is ready
#endif

	SubmitGraphicsTask(std::bind(&SGfx_Renderer::PreRenderUpdates, this), prepareData.mPreRenderUpdatesEvent);

	SubmitGraphicsTask(std::bind(&SGfx_Renderer::RenderPrePass, this), prepareData.mPrePassEvent);

	SubmitComputeTask(std::bind(&SGfx_Renderer::ComputeAmbientOcclusion, this), prepareData.mAmbientOcclusionEvent);

	SubmitGraphicsTask(std::bind(&SGfx_Renderer::RenderShadows, this), prepareData.mShadowsEvent);

	SubmitGraphicsTask(std::bind(&SGfx_Renderer::RenderOpaque, this), prepareData.mRenderOpaqueEvent);
	SubmitGraphicsTask(std::bind(&SGfx_Renderer::RenderDebugObjects, this), prepareData.mRenderDebugObjectsEvent);
	SubmitGraphicsTask(std::bind(&SGfx_Renderer::ComputePostEffects, this), prepareData.mPostEffectsEvent);

	for (SC_Event* taskEvent : mSubmittedTaskEvents)
		taskEvent->Wait();
	mSubmittedTaskEvents.RemoveAll();

	lastFrameFence = SR_RenderDevice::gInstance->GetGraphicsCommandQueue()->InsertFence();
	mCurrentView = nullptr;
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

SGfx_ShadowSystem* SGfx_Renderer::GetShadowMapSystem() const
{
	return mShadowMapSystem.get();
}

SGfx_AmbientOcclusion* SGfx_Renderer::GetAmbientOcclusion() const
{
	return mAmbientOcclusion.get();
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

void SGfx_Renderer::SubmitGraphicsTask(SR_RenderTaskFunctionSignature aTask, SR_TaskEvent* aEvent)
{
	SR_RenderTaskManager* renderTaskManager = SR_RenderDevice::gInstance->GetRenderTaskManager(); 

	if (SR_RenderDevice::gInstance->GetSupportCaps().mEnableAsyncCompute)
		renderTaskManager->SubmitSplit(SR_CommandListType::Compute); // Insert a split in the async queue batches to allow for more precise fences

	renderTaskManager->SubmitTask(aTask, SR_CommandListType::Graphics, aEvent);
	mSubmittedTaskEvents.Add(&aEvent->mCPUEvent);
	mLatestTaskEvent = &aEvent->mCPUEvent;
}

void SGfx_Renderer::SubmitGraphicsTask(SR_RenderTaskFunctionSignature aTask, const SC_UniquePtr<SR_TaskEvent>& aEvent)
{
	SubmitGraphicsTask(aTask, aEvent.get());
}

void SGfx_Renderer::SubmitComputeTask(SR_RenderTaskFunctionSignature aTask, SR_TaskEvent* aEvent)
{
	SR_RenderTaskManager* renderTaskManager = SR_RenderDevice::gInstance->GetRenderTaskManager();

	if (SR_RenderDevice::gInstance->GetSupportCaps().mEnableAsyncCompute) 
		renderTaskManager->SubmitSplit(SR_CommandListType::Graphics); // Insert a split in the graphics queue batches to allow for more precise fences

	renderTaskManager->SubmitTask(aTask, (SR_RenderDevice::gInstance->GetSupportCaps().mEnableAsyncCompute) ? SR_CommandListType::Compute : SR_CommandListType::Graphics, aEvent);
	mSubmittedTaskEvents.Add(&aEvent->mCPUEvent);
	mLatestTaskEvent = &aEvent->mCPUEvent;
}

void SGfx_Renderer::SubmitComputeTask(SR_RenderTaskFunctionSignature aTask, const SC_UniquePtr<SR_TaskEvent>& aEvent)
{
	SubmitComputeTask(aTask, aEvent.get());
}

void SGfx_Renderer::SubmitCopyTask(SR_RenderTaskFunctionSignature aTask, SR_TaskEvent* aEvent)
{
	SR_RenderTaskManager* renderTaskManager = SR_RenderDevice::gInstance->GetRenderTaskManager();
	renderTaskManager->SubmitTask(aTask, SR_CommandListType::Copy, aEvent);
	mSubmittedTaskEvents.Add(&aEvent->mCPUEvent);
	mLatestTaskEvent = &aEvent->mCPUEvent;
}

void SGfx_Renderer::SubmitCopyTask(SR_RenderTaskFunctionSignature aTask, const SC_UniquePtr<SR_TaskEvent>& aEvent)
{
	SubmitCopyTask(aTask, aEvent.get());
}

void SGfx_Renderer::PreRenderUpdates()
{
	SGfx_ViewData& renderData = mCurrentView->GetMutableRenderData();
#if ENABLE_RAYTRACING
	renderData.mBuildRaytracingSceneEvent->mCPUEvent.Wait();
#endif

	SGfx_MaterialGPUDataBuffer::Get().UpdateBuffer();
	renderData.mSceneConstants.mMaterialInfoBufferIndex = SGfx_MaterialGPUDataBuffer::Get().GetBufferDescriptorIndex();
	mViewConstantsBuffer->UpdateData(0, &renderData.mSceneConstants, sizeof(SGfx_SceneConstants));

	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	cmdList->SetRootConstantBuffer(mViewConstantsBuffer.get(), 1);
	mEnvironment->ComputeScatteringLUTs(cmdList.get());
}

#if ENABLE_RAYTRACING

void SGfx_Renderer::ComputeRaytracingScene()
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	SGfx_ViewData& renderData = mCurrentView->GetMutableRenderData();

	if (renderData.mRaytracingInstances.IsEmpty())
		return;

	cmdList->BeginEvent("Build Raytracing Scene");

	mRaytracingScene = cmdList->BuildRaytracingBuffer(renderData.mRaytracingInstances, nullptr);
	renderData.mSceneConstants.mRaytracingSceneDescriptorIndex = mRaytracingScene->GetDescriptorHeapIndex();

	cmdList->EndEvent();
}
#endif

void SGfx_Renderer::RenderShadows()
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	mShadowMapSystem->GetCSM()->Generate(cmdList.get(), mCurrentView->GetRenderData());
}

void SGfx_Renderer::RenderPrePass()
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	const SGfx_ViewData& renderData = mCurrentView->GetRenderData();

	cmdList->BeginEvent("Render PrePass");

	SC_Array<SC_Pair<uint32, SR_Resource*>> transitions;
	transitions.Add(SC_Pair(SR_ResourceState_DepthWrite, mDepthStencil->GetResource()));
	transitions.Add(SC_Pair(SR_ResourceState_RenderTarget, mMotionVectors.mResource.get()));
	cmdList->TransitionBarrier(transitions);

	cmdList->ClearRenderTarget(mMotionVectors.mRenderTarget.get(), SC_Vector4(0));
	cmdList->ClearDepthStencil(mDepthStencil.get());
	cmdList->SetRenderTargets(0, nullptr, mDepthStencil.get());

	SR_Rect rect = 
	{
		0,
		0,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.x,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.y
	};

	cmdList->SetViewport(rect);
	cmdList->SetScissorRect(rect);

	cmdList->SetRootConstantBuffer(mViewConstantsBuffer.get(), 1);

	uint32 i = 0;
	mDrawInfoBuffers[0].Respace(renderData.mDepthQueue.Count());
	for (const SGfx_RenderObject& renderObj : renderData.mDepthQueue)
	{
		if (!mDrawInfoBuffers[0][i])
		{
			SR_BufferResourceProperties cbDesc;
			cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
			cbDesc.mElementCount = 1;
			cbDesc.mElementSize = sizeof(SGfx_MeshShadingDrawInfoStruct);
			mDrawInfoBuffers[0][i] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
		}

		if (renderObj.mOutputVelocity)
			cmdList->SetRenderTarget(mMotionVectors.mRenderTarget.get(), mDepthStencil.get());
		else
			cmdList->SetRenderTargets(0, nullptr, mDepthStencil.get());

		SGfx_MeshShadingDrawInfoStruct drawInfo;
		drawInfo.mTransform = renderObj.mTransform;
		drawInfo.mPrevTransform = renderObj.mPrevTransform;
		drawInfo.mVertexBufferDescriptorIndex = renderObj.mVertexBuffer->GetDescriptorHeapIndex();
		drawInfo.mMeshletBufferDescriptorIndex = renderObj.mMeshletBuffer->GetDescriptorHeapIndex();
		drawInfo.mVertexIndexBufferDescriptorIndex = renderObj.mVertexIndexBuffer->GetDescriptorHeapIndex();
		drawInfo.mPrimitiveIndexBufferDescriptorIndex = renderObj.mPrimitiveIndexBuffer->GetDescriptorHeapIndex();
		drawInfo.mMaterialIndex = renderObj.mMaterialIndex;
		mDrawInfoBuffers[0][i]->UpdateData(0, &drawInfo, sizeof(SGfx_MeshShadingDrawInfoStruct));
		cmdList->SetRootConstantBuffer(mDrawInfoBuffers[0][i].get(), 0);

		cmdList->SetShaderState(renderObj.mShader);
		uint32 groupCount = renderObj.mMeshletBuffer->GetProperties().mElementCount;
		cmdList->DispatchMesh(groupCount);
		++i;
	}

	transitions.RemoveAll();
	transitions.Add(SC_Pair(SR_ResourceState_DepthRead, mDepthStencil->GetResource()));
	transitions.Add(SC_Pair(SR_ResourceState_Read, mMotionVectors.mResource.get()));
	cmdList->TransitionBarrier(transitions);
	cmdList->EndEvent(); // Render PrePass
}

void SGfx_Renderer::ComputeAmbientOcclusion()
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	const SGfx_ViewData& renderData = mCurrentView->GetRenderData();

#if ENABLE_RAYTRACING
	if (renderData.mRaytracingInstances.IsEmpty())
		return;
#endif

	cmdList->WaitFor(renderData.mPrePassEvent);

	cmdList->BeginEvent("Ambient Occlusion");

	cmdList->SetRootConstantBuffer(mViewConstantsBuffer.get(), 1);
	mAmbientOcclusion->Render(cmdList.get(), mDepthStencilSRV, renderData);

	cmdList->EndEvent(); // Ambient Occlusion
}

void SGfx_Renderer::RenderOpaque()
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	const SGfx_ViewData& renderData = mCurrentView->GetRenderData();

	cmdList->WaitFor(renderData.mAmbientOcclusionEvent);

	cmdList->BeginEvent("Render Opaque");

	SC_Array<SC_Pair<uint32, SR_Resource*>> barriers;
	barriers.Add(SC_Pair(SR_ResourceState_RenderTarget, mSceneColor.mResource.get()));
	cmdList->TransitionBarrier(barriers);
	cmdList->ClearRenderTarget(mSceneColor.mRenderTarget.get(), SC_Vector4(0.f, 0.f, 0.f, 0.f));
	cmdList->SetRenderTarget(mSceneColor.mRenderTarget.get(), mDepthStencil.get());

	SR_Rect rect =
	{
		0,
		0,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.x,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.y
	};
	cmdList->SetViewport(rect);
	cmdList->SetScissorRect(rect);

	cmdList->SetRootConstantBuffer(mViewConstantsBuffer.get(), 1);

	uint32 i = 0;
	mDrawInfoBuffers[1].Respace(renderData.mOpaqueQueue.Count());
	for (const SGfx_RenderObject& renderObj : renderData.mOpaqueQueue)
	{
		if (!mDrawInfoBuffers[1][i])
		{
			SR_BufferResourceProperties cbDesc;
			cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
			cbDesc.mElementCount = 1;
			cbDesc.mElementSize = sizeof(SGfx_MeshShadingDrawInfoStruct);
			mDrawInfoBuffers[1][i] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
		}

		SGfx_MeshShadingDrawInfoStruct drawInfo;
		drawInfo.mTransform = renderObj.mTransform;
		drawInfo.mPrevTransform = renderObj.mPrevTransform;
		drawInfo.mVertexBufferDescriptorIndex = renderObj.mVertexBuffer->GetDescriptorHeapIndex();
		drawInfo.mMeshletBufferDescriptorIndex = renderObj.mMeshletBuffer->GetDescriptorHeapIndex();
		drawInfo.mVertexIndexBufferDescriptorIndex = renderObj.mVertexIndexBuffer->GetDescriptorHeapIndex();
		drawInfo.mPrimitiveIndexBufferDescriptorIndex = renderObj.mPrimitiveIndexBuffer->GetDescriptorHeapIndex();
		drawInfo.mMaterialIndex = renderObj.mMaterialIndex;
		drawInfo.aoTex = mAmbientOcclusion->GetTexture()->GetDescriptorHeapIndex();
		mDrawInfoBuffers[1][i]->UpdateData(0, &drawInfo, sizeof(SGfx_MeshShadingDrawInfoStruct));
		cmdList->SetRootConstantBuffer(mDrawInfoBuffers[1][i].get(), 0);

		cmdList->SetShaderState(renderObj.mShader);
		uint32 groupCount = renderObj.mMeshletBuffer->GetProperties().mElementCount;
		cmdList->DispatchMesh(groupCount);
		++i;
	}

	if (renderData.mSkybox)
		renderData.mSkybox->Render(cmdList.get());

	cmdList->EndEvent(); // Render Opaque
}

void SGfx_Renderer::RenderDebugObjects()
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	const SGfx_ViewData& renderData = mCurrentView->GetRenderData();
	cmdList->BeginEvent("DebugDraw");

	cmdList->TransitionBarrier(SR_ResourceState_RenderTarget, mDebugTarget.mResource.get());
	cmdList->ClearRenderTarget(mDebugTarget.mRenderTarget.get(), SC_Vector4(0.f, 0.f, 0.f, 0.f));

	SR_Rect rect =
	{
		0,
		0,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.x,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.y
	};
	cmdList->SetViewport(rect);
	cmdList->SetScissorRect(rect);

	cmdList->SetRenderTarget(mDebugTarget.mRenderTarget.get(), mDepthStencil.get());
	cmdList->SetRootConstantBuffer(mViewConstantsBuffer.get(), 1);

	mDebugRenderer->SetDrawGrid(mSettings.mDrawGridHelper);
	mDebugRenderer->Render(cmdList.get(), renderData);

	cmdList->TransitionBarrier(SR_ResourceState_Read, mDebugTarget.mResource.get());
	cmdList->EndEvent();
}

void SGfx_Renderer::ComputePostEffects()
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	const SGfx_ViewData& renderData = mCurrentView->GetRenderData();
	const SR_Rect screenRect =
	{
		0,
		0,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.x,
		(uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.y
	};

	cmdList->SetRootConstantBuffer(mViewConstantsBuffer.get(), 1);

	if (mSettings.mEnableTemporalAA && gEnableTemporalAA)
	{
		cmdList->BeginEvent("TAA");
		cmdList->TransitionBarrier(SR_ResourceState_UnorderedAccess, mSceneColor2.mResource.get());
		struct TAAConstants
		{
			uint32 mTextureDescriptorIndex;
			uint32 mHistoryTextureDescriptorIndex;
			uint32 mMotionVectorTextureDescriptorIndex;
			uint32 mDepthStencilTextureDescriptorIndex;
			uint32 mOutputTextureDescriptorIndex;
			SC_Vector4 mTargetResolutionAndRcp;
		} taaConstants;
		taaConstants.mTextureDescriptorIndex = mSceneColor.mTexture->GetDescriptorHeapIndex();
		taaConstants.mHistoryTextureDescriptorIndex = mHistoryBuffer.mTexture->GetDescriptorHeapIndex();
		taaConstants.mMotionVectorTextureDescriptorIndex = mMotionVectors.mTexture->GetDescriptorHeapIndex();
		taaConstants.mDepthStencilTextureDescriptorIndex = mDepthStencilSRV->GetDescriptorHeapIndex();
		taaConstants.mOutputTextureDescriptorIndex = mSceneColor2.mTextureRW->GetDescriptorHeapIndex();
		taaConstants.mTargetResolutionAndRcp = SC_Vector4((float)mSceneColor2.mResource->GetProperties().mSize.x, (float)mSceneColor2.mResource->GetProperties().mSize.y, 1.0f / mSceneColor2.mResource->GetProperties().mSize.x, 1.0f / mSceneColor2.mResource->GetProperties().mSize.y);

		if (!mPostEffectCBuffers[1])
		{
			SR_BufferResourceProperties cbDesc;
			cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
			cbDesc.mElementCount = 1;
			cbDesc.mElementSize = sizeof(TAAConstants);
			mPostEffectCBuffers[1] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
		}
		mPostEffectCBuffers[1]->UpdateData(0, &taaConstants, sizeof(TAAConstants));
		cmdList->SetRootConstantBuffer(mPostEffectCBuffers[1].get(), 0);
		cmdList->Dispatch(mTAAResolveShader.get(), SC_IntVector(screenRect.mRight, screenRect.mBottom, 1));

		SC_Array<SC_Pair<uint32, SR_Resource*>> barriers;
		barriers.Add(SC_Pair(SR_ResourceState_CopySrc, mSceneColor2.mResource.get()));
		barriers.Add(SC_Pair(SR_ResourceState_CopyDst, mHistoryBuffer.mResource.get()));
		cmdList->TransitionBarrier(barriers);

		cmdList->CopyResource(mHistoryBuffer.mResource.get(), mSceneColor2.mResource.get());

		barriers.RemoveAll();
		barriers.Add(SC_Pair(SR_ResourceState_Read, mSceneColor2.mResource.get()));
		barriers.Add(SC_Pair(SR_ResourceState_Read, mHistoryBuffer.mResource.get()));
		cmdList->TransitionBarrier(barriers);

		cmdList->EndEvent(); // TAA
	}
	// Tonemap and Composite
	cmdList->BeginEvent("Compute Post Effects");

	cmdList->BeginEvent("Bloom");
	mPostEffects->Render(mCurrentView, (mSettings.mEnableTemporalAA) ? mSceneColor2.mTexture.get() : mSceneColor.mTexture.get());
	cmdList->EndEvent();

	cmdList->BeginEvent("Tonemap");
	cmdList->TransitionBarrier(SR_ResourceState_UnorderedAccess, mScreenColor.mResource.get());

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

	if (!mPostEffectCBuffers[0])
	{
		SR_BufferResourceProperties cbDesc;
		cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
		cbDesc.mElementCount = 1;
		cbDesc.mElementSize = sizeof(DispatchInfo);
		mPostEffectCBuffers[0] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
	}

	mPostEffectCBuffers[0]->UpdateData(0, &dispatchInfo, sizeof(DispatchInfo));
	cmdList->SetRootConstantBuffer(mPostEffectCBuffers[0].get(), 0);
	cmdList->Dispatch(mTonemapShader.get(), SC_IntVector(screenRect.mRight, screenRect.mBottom, 1));

	cmdList->TransitionBarrier(SR_ResourceState_Read, mScreenColor.mResource.get());
	cmdList->EndEvent(); // tonemap

	cmdList->EndEvent(); // Compute Post Effects
}

