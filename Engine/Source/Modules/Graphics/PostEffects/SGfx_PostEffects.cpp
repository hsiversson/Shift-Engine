#include "SGfx_PostEffects.h"
#include "Graphics/View/SGfx_View.h"
#include "Graphics/Misc/SGfx_DefaultTextures.h"

SGfx_PostEffects::SGfx_PostEffects()
{

}

SGfx_PostEffects::~SGfx_PostEffects()
{

}

bool SGfx_PostEffects::Init()
{
	SR_ShaderCompileArgs compileArgs;
	compileArgs.mEntryPoint = "BloomBrightnessFilter";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/PostEffect_BloomBrightnessFilter.ssf";
	compileArgs.mType = SR_ShaderType::Compute;

	SR_ShaderStateProperties brightnessFilterShaderProps;
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, brightnessFilterShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &brightnessFilterShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;
	mBloomData.mBrightnessFilterShader = SR_RenderDevice::gInstance->CreateShaderState(brightnessFilterShaderProps);

	compileArgs.mEntryPoint = "BloomUpsample";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/PostEffect_BloomUpsample.ssf";
	SR_ShaderStateProperties bloomUpsampleShaderProps;
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, bloomUpsampleShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &bloomUpsampleShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;
	mBloomData.mUpsampleShader = SR_RenderDevice::gInstance->CreateShaderState(bloomUpsampleShaderProps);
	mBloomData.mEnabled = true;

	compileArgs.mEntryPoint = "StableDownsample";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/PostEffect_Downsample.ssf";
	SR_ShaderStateProperties downsampleShaderProps;
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, downsampleShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &downsampleShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;
	mDownsampleShader = SR_RenderDevice::gInstance->CreateShaderState(downsampleShaderProps);

	return true;
}

void SGfx_PostEffects::Render(SGfx_View* aView, SR_Texture* aScreenColor)
{
	mTempTextures.RemoveAll();
	mTempConstantBuffers.RemoveAll();

	if (mBloomData.mEnabled)
		RenderBloom(aView, aScreenColor);

	//RenderTonemap(aView);
}

SR_Texture* SGfx_PostEffects::GetBloomTexture() const
{
	return (mBloomData.mEnabled) ? mBloomData.mResult.get() : SGfx_DefaultTextures::GetBlack1x1().get();
}

void SGfx_PostEffects::RenderAverageLuminance()
{

}

void SGfx_PostEffects::RenderBloom(SGfx_View* aView, SR_Texture* aScreenColor)
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	const SGfx_ViewData& renderData = aView->GetRenderData();

	// Downsample first mip to quarter and apply brightness filter
	// Recusively generate mips
	// Upsample using tent filter and apply to prev mip

	const SC_IntVector2 targetSize = (aScreenColor->GetResourceProperties().mSize.XY() + 1) / 2;

	SR_TextureResourceProperties resourceProps;
	resourceProps.mSize = SC_IntVector(targetSize, 1);
	resourceProps.mFormat = aScreenColor->GetResourceProperties().mFormat;
	resourceProps.mAllowRenderTarget = true;
	resourceProps.mAllowUnorderedAccess = true;
	resourceProps.mType = SR_ResourceType::Texture2D;
	SC_Ref<SR_TextureResource> tempResource = SR_RenderDevice::gInstance->CreateTextureResource(resourceProps);
	SR_TempTexture tempTex = SR_RenderDevice::gInstance->CreateTempTexture(resourceProps, true, false, true);

	struct FilterConstants
	{
		SC_Vector4 mTargetResolutionAndRcp;
		float mThreshold;
		float mSoftThreshold;
		uint32 mInputTextureDescriptorIndex;
		uint32 mOutputTextureDescriptorIndex;
	} constants;
	constants.mTargetResolutionAndRcp = SC_Vector4(targetSize, SC_Vector2(1.0f / targetSize.x, 1.0f / targetSize.y));
	constants.mThreshold = 1.0f;
	constants.mSoftThreshold = 1.0f;
	constants.mInputTextureDescriptorIndex = aScreenColor->GetDescriptorHeapIndex();
	constants.mOutputTextureDescriptorIndex = tempTex.mRWTexture->GetDescriptorHeapIndex();

	SR_BufferResourceProperties cbProps;
	cbProps.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
	cbProps.mElementCount = sizeof(FilterConstants);
	cbProps.mElementSize = 1;
	SC_Ref<SR_BufferResource> cb = mTempConstantBuffers.Add(SR_RenderDevice::gInstance->CreateBufferResource(cbProps));
	cb->UpdateData(0, &constants, sizeof(constants));
	cmdList->SetRootConstantBuffer(cb.get(), 0);
	cmdList->Dispatch(mBloomData.mBrightnessFilterShader.get(), targetSize.x, targetSize.y);
	cmdList->UnorderedAccessBarrier(tempTex.mResource.get());

	SR_TempTexture mipResult;
	RenderBloomMipRecursive(renderData, mipResult, tempTex.mTexture.get());

	SR_TempTexture result;
	UpsampleBloomMip(result, tempTex.mTexture.get(), mipResult);

	if (!mBloomData.mResult)
	{
		SC_Ref<SR_TextureResource> res = SR_RenderDevice::gInstance->CreateTextureResource(result.mResource->GetProperties());
		mBloomData.mResult = SR_RenderDevice::gInstance->CreateTexture(result.mResource->GetProperties().mFormat, res);
	}
	cmdList->CopyResource(mBloomData.mResult->GetResource(), result.mResource.get());
}

void SGfx_PostEffects::RenderBloomMipRecursive(const SGfx_ViewData& aRenderData, SR_TempTexture& aOutMip, SR_Texture* aInMip)
{
	SR_TempTexture downsampledMip = Downsample(aInMip);
	SR_Texture* downsampledMipTex = downsampledMip.mTexture.get();

	int32 maxResolutionSize = SC_Max(downsampledMipTex->GetResourceProperties().mSize.x, downsampledMipTex->GetResourceProperties().mSize.y);
	if (maxResolutionSize > 4)
	{
		SR_TempTexture mipResult;
		RenderBloomMipRecursive(aRenderData, mipResult, downsampledMipTex);
		UpsampleBloomMip(aOutMip, downsampledMipTex, mipResult);
	}
	else
		aOutMip = downsampledMip;
}

void SGfx_PostEffects::UpsampleBloomMip(SR_TempTexture& aOutMip, SR_Texture* aFullMip, SR_TempTexture& aDownsampledMip)
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	SC_IntVector fullSize = aFullMip->GetResourceProperties().mSize;
	SC_IntVector upsampleSize = fullSize;
	SR_Texture* downsampledMipTex = aDownsampledMip.mTexture.get();

	SR_TextureResourceProperties resourceProps;
	resourceProps.mAllowUnorderedAccess = true;
	resourceProps.mSize = fullSize;
	resourceProps.mFormat = aFullMip->GetResourceProperties().mFormat;
	resourceProps.mDebugName = "Bloom Upsample";
	resourceProps.mType = SR_ResourceType::Texture2D;
	aOutMip = SR_RenderDevice::gInstance->CreateTempTexture(resourceProps, true, false, true);

	struct Constants
	{
		SC_Vector2 mMinUV;
		SC_Vector2 mMaxUV;
		SC_Vector2 mTexelSizeAndUVScale;
		SC_Vector2 mTentFilterScale;
		SC_IntVector2 mDestOffset;
		float mStrength;
		uint32 mInput0TextureDescriptorIndex;
		uint32 mInput1TextureDescriptorIndex;
		uint32 mOutputTextureDescriptorIndex;
	} constants;

	constants.mMinUV = SC_Vector2(0.0f);
	constants.mMaxUV = SC_Vector2(1.0f);
	constants.mTexelSizeAndUVScale = SC_Vector2(1.0f) / fullSize.XY();
	constants.mTentFilterScale = SC_Vector2(1.75f, 1.25f);
	constants.mDestOffset = SC_Vector2(0.0f);
	constants.mStrength = 0.63f;
	constants.mInput0TextureDescriptorIndex = downsampledMipTex->GetDescriptorHeapIndex();
	constants.mInput1TextureDescriptorIndex = aFullMip->GetDescriptorHeapIndex();
	constants.mOutputTextureDescriptorIndex = aOutMip.mRWTexture->GetDescriptorHeapIndex();

	SR_BufferResourceProperties cbProps;
	cbProps.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
	cbProps.mElementCount = sizeof(Constants);
	cbProps.mElementSize = 1;
	SC_Ref<SR_BufferResource> cb = mTempConstantBuffers.Add(SR_RenderDevice::gInstance->CreateBufferResource(cbProps));
	cb->UpdateData(0, &constants, sizeof(constants));
	cmdList->SetRootConstantBuffer(cb.get(), 0);

	cmdList->Dispatch(mBloomData.mUpsampleShader.get(), SC_IntVector(upsampleSize.XY(), 1));
	cmdList->UnorderedAccessBarrier(aOutMip.mResource.get());
}

SR_TempTexture SGfx_PostEffects::Downsample(SR_Texture* aSource)
{
	// TODO: Make viewport aware
	const SC_IntVector2 fullSize = aSource->GetResourceProperties().mSize.XY();
	SC_IntVector2 targetSize = (fullSize + 1) / 2;

	SR_TextureResourceProperties resourceProps;
	resourceProps.mSize = SC_IntVector(targetSize, 1);
	resourceProps.mFormat = aSource->GetResourceProperties().mFormat;
	resourceProps.mAllowRenderTarget = true;
	resourceProps.mAllowUnorderedAccess = true;
	resourceProps.mType = SR_ResourceType::Texture2D;
	SR_TempTexture tempTex = SR_RenderDevice::gInstance->CreateTempTexture(resourceProps, true, false, true);

	struct Constants
	{
		SC_Vector2 mMinUV;
		SC_Vector2 mMaxUV;

		SC_Vector2 mTexelSizeAndUVScale;
		SC_Vector2 mUVOffset;

		SC_Vector2 m2DivTexelSizeAndUVScale;
		uint32 mInputTextureDescriptorIndex;
		uint32 mOutputTextureDescriptorIndex;
	} constants;
	constants.mMinUV = SC_Vector2(0.0f);
	constants.mMaxUV = SC_Vector2(1.0f);
	constants.mTexelSizeAndUVScale = SC_Vector2(1.0f) / fullSize;
	constants.mUVOffset = SC_Vector2(0.0f);
	constants.m2DivTexelSizeAndUVScale = SC_Vector2(2.0f) / constants.mTexelSizeAndUVScale;
	constants.mInputTextureDescriptorIndex = aSource->GetDescriptorHeapIndex();
	constants.mOutputTextureDescriptorIndex = tempTex.mRWTexture->GetDescriptorHeapIndex();

	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();

	SR_BufferResourceProperties cbProps;
	cbProps.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
	cbProps.mElementCount = sizeof(Constants);
	cbProps.mElementSize = 1;
	SC_Ref<SR_BufferResource> cb = mTempConstantBuffers.Add(SR_RenderDevice::gInstance->CreateBufferResource(cbProps));
	cb->UpdateData(0, &constants, sizeof(constants));
	cmdList->SetRootConstantBuffer(cb.get(), 0);

	cmdList->Dispatch(mDownsampleShader.get(), targetSize.x, targetSize.y);
	cmdList->UnorderedAccessBarrier(tempTex.mResource.get());

	return tempTex;
}

void SGfx_PostEffects::RenderLensFlare()
{

}

void SGfx_PostEffects::RenderChromaticAbberation()
{

}

void SGfx_PostEffects::RenderTonemap(SGfx_View* aView)
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	const SGfx_ViewData& renderData = aView->GetRenderData();
	const SR_Rect screenRect = { 0, 0, (uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.x, (uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.y };

	//struct DispatchInfo
	//{
	//	uint32 mSceneColorDescriptorIndex;
	//	uint32 mOutputTextureDescriptorIndex;
	//	//uint32 mBloomTextureDescriptorIndex;
	//} dispatchInfo;


	//cmdList->SetRootConstantBuffer(mPostEffectCBuffers[0].get(), 0);
	cmdList->Dispatch(mTonemapData.mShader.get(), SC_IntVector(screenRect.mRight, screenRect.mBottom, 1));
}
