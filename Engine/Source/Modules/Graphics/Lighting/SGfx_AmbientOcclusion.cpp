#include "SGfx_AmbientOcclusion.h"
#include "Graphics/View/SGfx_ViewData.h"
#include "Graphics/Misc/SGfx_DefaultTextures.h"

struct RTAOConstants
{
	SC_Vector4 mTargetSizeAndInvSize;
	uint32 mNumRaysPerPixel;
	float mRadius;
	uint32 mDepthBufferDescriptorIndex;
	uint32 mOutputTextureDescriptorIndex;
};

struct DenoiserConstants
{
	uint32 mInputTexture;
	uint32 mOutputTexture;
	uint32 mDepthBufferTexture;
	uint32 _pad;
	SC_Vector2 mBlurDirection;
};

SGfx_AmbientOcclusion::SGfx_AmbientOcclusion()
{

}

SGfx_AmbientOcclusion::~SGfx_AmbientOcclusion()
{

}

bool SGfx_AmbientOcclusion::Init()
{
#if SR_ENABLE_RAYTRACING
	SR_ShaderStateProperties shaderProps;
	SR_ShaderCompileArgs compileArgs;
	compileArgs.mEntryPoint = "Main";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/RTAO.ssf";
	compileArgs.mType = SR_ShaderType::Raytracing;
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, shaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Raytracing)], &shaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Raytracing)]))
		return false;

	SR_ShaderStateProperties::RTHitGroup& hitGroup = shaderProps.mHitGroups.Add();
	hitGroup.myHasClosestHit = true;
	hitGroup.myHasAnyHit = false;
	hitGroup.myHasIntersection = false;

	mRTAOShader = SR_RenderDevice::gInstance->CreateShaderState(shaderProps);

	SR_ShaderStateProperties shaderProps2;
	compileArgs.mEntryPoint = "Main";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/RTAODenoise.ssf";
	compileArgs.mType = SR_ShaderType::Compute;
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, shaderProps2.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &shaderProps2.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;

	mDenoiseShader = SR_RenderDevice::gInstance->CreateShaderState(shaderProps2);
#endif //SR_ENABLE_RAYTRACING

	SR_TextureResourceProperties textureResourceProps;
	textureResourceProps.mSize = SC_IntVector(SC_Vector2(SR_RenderDevice::gInstance->GetSwapChain()->GetProperties().mSize) * 0.75f, 1);
	textureResourceProps.mFormat = SR_Format::R8_UNORM;
	textureResourceProps.mType = SR_ResourceType::Texture2D;
	textureResourceProps.mAllowUnorderedAccess = true;
	textureResourceProps.mNumMips = 1;

	textureResourceProps.mDebugName = "Raw AO";
	SC_Ref<SR_TextureResource> rawResource = SR_RenderDevice::gInstance->CreateTextureResource(textureResourceProps);

	textureResourceProps.mDebugName = "Denoised AO";
	SC_Ref<SR_TextureResource> denoisedResource0 = SR_RenderDevice::gInstance->CreateTextureResource(textureResourceProps);
	SC_Ref<SR_TextureResource> denoisedResource1 = SR_RenderDevice::gInstance->CreateTextureResource(textureResourceProps);

	SR_TextureProperties textureProps(textureResourceProps.mFormat);
	mRawOutputTexture = SR_RenderDevice::gInstance->CreateTexture(textureProps, rawResource);
	mDenoisedTexture[0] = SR_RenderDevice::gInstance->CreateTexture(textureProps, denoisedResource0);
	mDenoisedTexture[1] = SR_RenderDevice::gInstance->CreateTexture(textureProps, denoisedResource1);

	textureProps.mWritable = true;
	mRawOutputRWTexture = SR_RenderDevice::gInstance->CreateTexture(textureProps, rawResource);
	mDenoisedRWTexture[0] = SR_RenderDevice::gInstance->CreateTexture(textureProps, denoisedResource0);
	mDenoisedRWTexture[1] = SR_RenderDevice::gInstance->CreateTexture(textureProps, denoisedResource1);

	SR_BufferResourceProperties cbDesc;
	cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
	cbDesc.mElementCount = 1;
	cbDesc.mElementSize = sizeof(RTAOConstants);
	mConstantBuffer = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);

	cbDesc.mElementSize = sizeof(DenoiserConstants);
	mDenoiserConstantBuffer[0] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
	mDenoiserConstantBuffer[1] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);

	return true;
}

void SGfx_AmbientOcclusion::Render(SR_CommandList* aCmdList, const SC_Ref<SR_Texture>& aDepthBuffer, const SGfx_ViewData& aRenderData)
{
#if SR_ENABLE_RAYTRACING
	if (mRTAOSettings.mAOType == Type::RTAO)
	{
		RenderRTAO(aCmdList, aDepthBuffer, aRenderData);
	}
	else
#endif
	{
		if (mRTAOSettings.mAOType == Type::GTAO)
		{
			RenderGTAO(aCmdList, aDepthBuffer, aRenderData);
		}
		else
			return;
	}
}

SR_Texture* SGfx_AmbientOcclusion::GetTexture() const
{
	if (mRTAOSettings.mAOType == Type::None)
		return SGfx_DefaultTextures::GetWhite1x1();

	return mRTAOSettings.mUseDenoiser ? mDenoisedTexture[1] : mRawOutputTexture;
}

const SGfx_AmbientOcclusion::RTAOSettings& SGfx_AmbientOcclusion::GetRTAOSettings() const
{
	return mRTAOSettings;
}

SGfx_AmbientOcclusion::RTAOSettings& SGfx_AmbientOcclusion::GetRTAOSettings()
{
	return mRTAOSettings;
}

void SGfx_AmbientOcclusion::RenderGTAO(SR_CommandList* /*aCmdList*/, const SC_Ref<SR_Texture>& /*aDepthBuffer*/, const SGfx_ViewData& /*aRenderData*/)
{
}

#if SR_ENABLE_RAYTRACING
void SGfx_AmbientOcclusion::RenderRTAO(SR_CommandList* aCmdList, const SC_Ref<SR_Texture>& aDepthBuffer, const SGfx_ViewData& aRenderData)
{
	SC_IntVector targetSize = mRawOutputRWTexture->GetResource()->GetProperties().mSize;

	aCmdList->BeginEvent("Trace Rays");

	RTAOConstants constants;
	constants.mTargetSizeAndInvSize = SC_Vector4((float)targetSize.x, (float)targetSize.y, 1.0f / (float)targetSize.x, 1.0f / (float)targetSize.y);
	constants.mNumRaysPerPixel = mRTAOSettings.mNumRaysPerPixel;
	constants.mRadius = mRTAOSettings.mRadius;
	constants.mDepthBufferDescriptorIndex = aDepthBuffer->GetDescriptorHeapIndex();
	constants.mOutputTextureDescriptorIndex = mRawOutputRWTexture->GetDescriptorHeapIndex();

	mConstantBuffer->UpdateData(0, &constants, sizeof(RTAOConstants));

	aCmdList->SetRootConstantBuffer(mConstantBuffer, 0);
	aCmdList->SetShaderState(mRTAOShader);
	aCmdList->DispatchRays(SC_IntVector(constants.mTargetSizeAndInvSize.XY() * aRenderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.ZW(), 1));

	aCmdList->EndEvent();

	aCmdList->UnorderedAccessBarrier(mRawOutputRWTexture->GetResource());

	if (mRTAOSettings.mUseDenoiser)
	{
		aCmdList->BeginEvent("Denoise");

		// Denoise
		DenoiserConstants denoiseConstants;
		denoiseConstants.mInputTexture = mRawOutputTexture->GetDescriptorHeapIndex();
		denoiseConstants.mOutputTexture = mDenoisedRWTexture[0]->GetDescriptorHeapIndex();
		denoiseConstants.mDepthBufferTexture = aDepthBuffer->GetDescriptorHeapIndex();
		denoiseConstants.mBlurDirection = SC_Vector2(1.0f, 0.0f);
		mDenoiserConstantBuffer[0]->UpdateData(0, &denoiseConstants, sizeof(DenoiserConstants));

		aCmdList->SetRootConstantBuffer(mDenoiserConstantBuffer[0], 0);
		aCmdList->Dispatch(mDenoiseShader, SC_IntVector(constants.mTargetSizeAndInvSize.XY() * aRenderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.ZW(), 1));

		denoiseConstants.mInputTexture = mDenoisedRWTexture[0]->GetDescriptorHeapIndex();
		denoiseConstants.mOutputTexture = mDenoisedRWTexture[1]->GetDescriptorHeapIndex();
		denoiseConstants.mBlurDirection = SC_Vector2(0.0f, 1.0f);
		mDenoiserConstantBuffer[1]->UpdateData(0, &denoiseConstants, sizeof(DenoiserConstants));
		aCmdList->SetRootConstantBuffer(mDenoiserConstantBuffer[1], 0);
		aCmdList->Dispatch(mDenoiseShader, SC_IntVector(constants.mTargetSizeAndInvSize.XY() * aRenderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.ZW(), 1));

		aCmdList->EndEvent();
	}
}
#endif