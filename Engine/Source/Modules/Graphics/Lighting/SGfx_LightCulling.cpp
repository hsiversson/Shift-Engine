#include "SGfx_LightCulling.h"
#include "../View/SGfx_ViewData.h"

SGfx_LightCulling::SGfx_LightCulling()
{

}

SGfx_LightCulling::~SGfx_LightCulling()
{

}

bool SGfx_LightCulling::Init()
{
	SR_ShaderCompileArgs compileArgs;
	compileArgs.mEntryPoint = "CullLights";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/LightCulling.ssf";
	compileArgs.mType = SR_ShaderType::Compute;

	SR_ShaderStateProperties shaderProps;
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, shaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &shaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;
	mLightCullingShader = SR_RenderDevice::gInstance->CreateShaderState(shaderProps);

    return true;
}

void SGfx_LightCulling::Prepare(SGfx_ViewData& aPrepareData)
{
	aPrepareData.mPrepareCullLightsEvent.Wait();

	SC_PROFILER_FUNCTION();
	mDelayedDeleteResources.RemoveAll();
	//const uint32 totalLightCount = aPrepareData.mVisibleLights.Count();

	const SC_IntVector2 resolution = aPrepareData.mSceneConstants.mViewConstants.mViewportSizeAndScale.XY();
	const SC_IntVector2 numLightTilesCPU = SC_Vector2(resolution + gTileSize - 1) / (float)gTileSize;
	const uint32 totalLightTilesCPU = numLightTilesCPU.x * numLightTilesCPU.y;

	mLightTiles.Respace(totalLightTilesCPU);
	mLightTileFrustums.Respace(totalLightTilesCPU);

	for (uint32 i = 0; i < totalLightTilesCPU; ++i) // Build CPU side light tiles and pre-cull the lights
	{

	}


	mConstants.mNumTiles = SC_Vector2(resolution + gTileSize - 1) / (float)gTileSize;
    mConstants.mTotalNumLights = aPrepareData.mVisibleLights.Count();

    if (mConstants.mTotalNumLights > 0)
    {
		SC_Array<SGfx_Light::LocalLightShaderData> gpuData;
		gpuData.Reserve(mConstants.mTotalNumLights);

		for (const SGfx_LightRenderData& lightData : aPrepareData.mVisibleLights)
			gpuData.Add(lightData.mGPUData);

        SR_BufferResourceProperties lightBufferResourceProps;
        lightBufferResourceProps.mBindFlags = SR_BufferBindFlag_Buffer;
        lightBufferResourceProps.mWritable = false;
        lightBufferResourceProps.mElementSize = gpuData.ElementStride();
        lightBufferResourceProps.mElementCount = gpuData.Count();
		lightBufferResourceProps.mDebugName = "LightBuffer";
        SC_Ref<SR_BufferResource> lightBufferResource = SR_RenderDevice::gInstance->CreateBufferResource(lightBufferResourceProps, gpuData.GetBuffer());

		if (mLightBuffer)
			mDelayedDeleteResources.Add(mLightBuffer);

        SR_BufferProperties lightBufferProps;
        lightBufferProps.mFirstElement = 0;
        lightBufferProps.mElementCount = lightBufferResourceProps.mElementCount;
        lightBufferProps.mType = SR_BufferType::Structured;
        lightBufferProps.mWritable = false;
        mLightBuffer = SR_RenderDevice::gInstance->CreateBuffer(lightBufferProps, lightBufferResource);

	    mConstants.mLightBufferDescriptorIndex = mLightBuffer->GetDescriptorHeapIndex();
    }

	SR_BufferResourceProperties tileGridResourceProps;
	tileGridResourceProps.mBindFlags = SR_BufferBindFlag_Buffer;
	tileGridResourceProps.mWritable = true;
	tileGridResourceProps.mElementSize = sizeof(uint32) * (gNumLightsPerTile + 1); // +1 is for num active lights count
	tileGridResourceProps.mElementCount = mConstants.mNumTiles.x * mConstants.mNumTiles.y;
	tileGridResourceProps.mDebugName = "TileGrid";
	SC_Ref<SR_BufferResource> tileGridResource = SR_RenderDevice::gInstance->CreateBufferResource(tileGridResourceProps);

	if (mTileGridBuffer)
		mDelayedDeleteResources.Add(mTileGridBuffer);

	SR_BufferProperties tileGridProps;
	tileGridProps.mFirstElement = 0;
	tileGridProps.mElementCount = tileGridResourceProps.mElementCount;
	tileGridProps.mType = SR_BufferType::Structured;
	tileGridProps.mWritable = false;
	mTileGridBuffer = SR_RenderDevice::gInstance->CreateBuffer(tileGridProps, tileGridResource);
	tileGridProps.mWritable = true;
	mTileGridBufferRW = SR_RenderDevice::gInstance->CreateBuffer(tileGridProps, tileGridResource);

	mConstants.mTileGridDescriptorIndex = mTileGridBuffer->GetDescriptorHeapIndex();
	aPrepareData.mSceneConstants.mLightCullingConstants = mConstants;
}

void SGfx_LightCulling::CullLights(SR_CommandList* aCmdList, const SGfx_ViewData& aRenderData, const SR_Texture* aDepthBuffer)
{
    if (mConstants.mTotalNumLights == 0)
        return;

    aCmdList->BeginEvent("Cull Lights");
    const SC_IntVector2 resolution = aRenderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.XY();

    struct Constants
	{
		uint32 mDepthBufferDescriptorIndex;
		uint32 mTileGridRWDescriptorIndex;
    } constants;
    constants.mDepthBufferDescriptorIndex = aDepthBuffer->GetDescriptorHeapIndex();
    constants.mTileGridRWDescriptorIndex = mTileGridBufferRW->GetDescriptorHeapIndex();

    if (!mCullConstants)
	{
		SR_BufferResourceProperties cbDesc;
		cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
		cbDesc.mElementCount = 1;
		cbDesc.mElementSize = sizeof(Constants);
        mCullConstants = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
    }

    mCullConstants->UpdateData(0, &constants, sizeof(constants));
    aCmdList->SetRootConstantBuffer(mCullConstants, 0);

    aCmdList->Dispatch(mLightCullingShader, resolution.x, resolution.y);
    aCmdList->EndEvent();
}

const SGfx_LightCullingConstants& SGfx_LightCulling::GetConstants() const
{
    return mConstants;
}

SR_Buffer* SGfx_LightCulling::GetLightBuffer() const
{
    return mLightBuffer;
}

SR_Buffer* SGfx_LightCulling::GetTileGridBuffer() const
{
	return mTileGridBuffer;
}

void SGfx_LightCulling::CullLightsCPU()
{
}
