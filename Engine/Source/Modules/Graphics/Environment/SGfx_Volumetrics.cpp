#include "SGfx_Volumetrics.h"

SGfx_Volumetrics::SGfx_Volumetrics()
{

}

SGfx_Volumetrics::~SGfx_Volumetrics()
{

}

bool SGfx_Volumetrics::Init()
{
    SR_TextureResourceProperties volumetricScatteringResourceDesc;
    volumetricScatteringResourceDesc.mSize = mFogSettings.mResolution;
    volumetricScatteringResourceDesc.mAllowUnorderedAccess = true;
    volumetricScatteringResourceDesc.mFormat = SR_Format::RGBA16_FLOAT;
    volumetricScatteringResourceDesc.mType = SR_ResourceType::Texture3D;
    volumetricScatteringResourceDesc.mDebugName = "VolumetricScatteringBuffer";
    SC_Ref<SR_TextureResource> volumetricScatteringResource = SR_RenderDevice::gInstance->CreateTextureResource(volumetricScatteringResourceDesc);

    SR_TextureProperties volumetricScatteringDesc;
    volumetricScatteringDesc.mDimension = SR_TextureDimension::Texture3D;
    volumetricScatteringDesc.mFormat = SR_Format::RGBA16_FLOAT;
    mVolumetricScattering = SR_RenderDevice::gInstance->CreateTexture(volumetricScatteringDesc, volumetricScatteringResource);

	volumetricScatteringDesc.mWritable = true;
	mVolumetricScatteringRW = SR_RenderDevice::gInstance->CreateTexture(volumetricScatteringDesc, volumetricScatteringResource);


    return true;
}

void SGfx_Volumetrics::ComputeVolumetricFog()
{
    //SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
}

void SGfx_Volumetrics::ComputeVolumetricClouds()
{
	//SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();

}

void SGfx_Volumetrics::ApplyVolumetrics()
{
	//SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();

}
