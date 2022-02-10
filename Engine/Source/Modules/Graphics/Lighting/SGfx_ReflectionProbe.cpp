#include "SGfx_ReflectionProbe.h"
#include "View/SGfx_ViewData.h"
#include "Renderer/SGfx_DrawInfo.h"


SGfx_ReflectionProbe::SGfx_ReflectionProbe()
	: mPosition(0.0f, 2.0f, 0.0f)
	, mResolution(512, 512)
	, mDistance(100.0f)
{
	CreateResources();
}

SGfx_ReflectionProbe::~SGfx_ReflectionProbe()
{

}

void SGfx_ReflectionProbe::Render(SR_CommandList* aCmdList, const SGfx_ViewData& aRenderData)
{
	SGfx_Camera camera;
	camera.SetPerspectiveProjection({ (float)mResolution.x, (float)mResolution.y }, 0.1f, mDistance, 90.f);
	camera.SetPosition(mPosition);

	static const SC_Vector targets[6] =
	{
		SC_Vector(1, 0, 0),
		SC_Vector(-1, 0, 0),
		SC_Vector(0, 1, 0),
		SC_Vector(0, -1, 0),
		SC_Vector(0, 0, 1),
		SC_Vector(0, 0, -1),
	};

	aCmdList->BeginEvent("Reflection Probe");

	for (uint32 i = 0; i < 6; ++i)
	{
		SC_Vector up = { 0.0f, 1.0f, 0.0f };
		if (i == 2)
			up = { 0.0f, 0.0f, -1.0f };
		else if (i == 3)
			up = { 0.0f, 0.0f, 1.0f };

		camera.LookAt(targets[i], up);
		RenderFace(aCmdList, i, camera, aRenderData);
	}

	aCmdList->EndEvent();
}

SR_Texture* SGfx_ReflectionProbe::GetTexture() const
{
	return mProbeCubemap.get();
}

bool SGfx_ReflectionProbe::CreateResources()
{
	SR_BufferResourceProperties cbDesc;
	cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
	cbDesc.mElementCount = 1;
	cbDesc.mElementSize = sizeof(SGfx_SceneConstants);
	mSceneConstantsBuffer[0] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
	mSceneConstantsBuffer[1] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
	mSceneConstantsBuffer[2] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
	mSceneConstantsBuffer[3] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
	mSceneConstantsBuffer[4] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
	mSceneConstantsBuffer[5] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);

	SR_TextureResourceProperties depthStencilResourceProps;
	depthStencilResourceProps.mSize = SC_IntVector(mResolution);
	depthStencilResourceProps.mFormat = SR_Format::D32_FLOAT;
	depthStencilResourceProps.mNumMips = 1;
	depthStencilResourceProps.mType = SR_ResourceType::Texture2D;
	depthStencilResourceProps.mAllowDepthStencil = true;
	depthStencilResourceProps.mAllowRenderTarget = false;
	depthStencilResourceProps.mAllowUnorderedAccess = false;
	SC_Ref<SR_TextureResource> depthStencilResource = SR_RenderDevice::gInstance->CreateTextureResource(depthStencilResourceProps);

	SR_DepthStencilProperties dsvProperties(SR_Format::D32_FLOAT);
	mDepthStencil = SR_RenderDevice::gInstance->CreateDepthStencil(dsvProperties, depthStencilResource);

	SR_TextureResourceProperties resourceProps;
	resourceProps.mAllowRenderTarget = true;
	resourceProps.mAllowUnorderedAccess = false;
	resourceProps.mFormat = SR_Format::RG11B10_FLOAT;
	resourceProps.mSize = SC_IntVector(mResolution);
	resourceProps.mArraySize = 1;
	resourceProps.mNumMips = 1;
	resourceProps.mType = SR_ResourceType::Texture2D;
	resourceProps.mDebugName = "ReflectionProbe";
	resourceProps.mIsCubeMap = true;

	SC_Ref<SR_TextureResource> cubemapResource = SR_RenderDevice::gInstance->CreateTextureResource(resourceProps);

	for (uint32 i = 0; i < 6; ++i)
	{
		SR_RenderTargetProperties rtProps;
		rtProps.mFormat = SR_Format::RG11B10_FLOAT;
		rtProps.mFace = SR_CubeMapFace(i);

		mProbeRTs[i] = SR_RenderDevice::gInstance->CreateRenderTarget(rtProps, cubemapResource);
	}

	return true;
}

void SGfx_ReflectionProbe::RenderFace(SR_CommandList* aCmdList, uint32 aFaceIndex, const SGfx_Camera& aCamera, const SGfx_ViewData& aRenderData)
{
	std::string eventTag = std::string("Face") + std::to_string(aFaceIndex);
	aCmdList->BeginEvent(eventTag.c_str());

	SC_Array<SC_Pair<uint32, SR_Resource*>> barriers;
	barriers.Add(SC_Pair(SR_ResourceState_RenderTarget, mProbeRTs[aFaceIndex]->GetResource()));
	barriers.Add(SC_Pair(SR_ResourceState_DepthWrite, mDepthStencil->GetResource()));
	aCmdList->TransitionBarrier(barriers);
	aCmdList->ClearRenderTarget(mProbeRTs[aFaceIndex].get(), SC_Vector4(0.0f));
	aCmdList->ClearDepthStencil(mDepthStencil.get());
	aCmdList->SetRenderTarget(mProbeRTs[aFaceIndex].get(), mDepthStencil.get());

	SGfx_SceneConstants sceneConstants = aRenderData.mSceneConstants;
	sceneConstants.mViewConstants = aCamera.GetViewConstants();
	mSceneConstantsBuffer[aFaceIndex]->UpdateData(0, &sceneConstants, sizeof(SGfx_SceneConstants));
	aCmdList->SetRootConstantBuffer(mSceneConstantsBuffer[aFaceIndex].get(), 1);

	SR_Rect rect =
	{
		0,
		0,
		(uint32)sceneConstants.mViewConstants.mViewportSizeAndScale.x,
		(uint32)sceneConstants.mViewConstants.mViewportSizeAndScale.y
	};

	aCmdList->SetViewport(rect);
	aCmdList->SetScissorRect(rect);

	uint32 i = 0;
	mDrawInfoBuffers[aFaceIndex].Respace(aRenderData.mOpaqueQueue.Count());
	for (const SGfx_RenderObject& renderObj : aRenderData.mOpaqueQueue)
	{
		if (!mDrawInfoBuffers[aFaceIndex][i])
		{
			SR_BufferResourceProperties cbDesc;
			cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
			cbDesc.mElementCount = 1;
			cbDesc.mElementSize = sizeof(SGfx_MeshShadingDrawInfoStruct);
			mDrawInfoBuffers[aFaceIndex][i] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
		}

		SGfx_MeshShadingDrawInfoStruct drawInfo;
		drawInfo.mTransform = renderObj.mTransform;
		drawInfo.mPrevTransform = renderObj.mPrevTransform;
		drawInfo.mVertexBufferDescriptorIndex = renderObj.mVertexBuffer->GetDescriptorHeapIndex();
		drawInfo.mMeshletBufferDescriptorIndex = renderObj.mMeshletBuffer->GetDescriptorHeapIndex();
		drawInfo.mVertexIndexBufferDescriptorIndex = renderObj.mVertexIndexBuffer->GetDescriptorHeapIndex();
		drawInfo.mPrimitiveIndexBufferDescriptorIndex = renderObj.mPrimitiveIndexBuffer->GetDescriptorHeapIndex();
		drawInfo.mMaterialIndex = renderObj.mMaterialIndex;
		mDrawInfoBuffers[aFaceIndex][i]->UpdateData(0, &drawInfo, sizeof(SGfx_MeshShadingDrawInfoStruct));
		aCmdList->SetRootConstantBuffer(mDrawInfoBuffers[aFaceIndex][i].get(), 0);

		aCmdList->SetShaderState(renderObj.mShader);
		uint32 groupCount = renderObj.mMeshletBuffer->GetProperties().mElementCount;
		aCmdList->DispatchMesh(groupCount);
		++i;
	}
	if (aRenderData.mSkybox)
		aRenderData.mSkybox->Render(aCmdList);

	aCmdList->EndEvent();
}
