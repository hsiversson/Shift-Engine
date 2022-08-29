
#include "SGfx_MaterialInstance.h"
#include "SGfx_MaterialCache.h"
#include "RenderCore/ShaderCompiler/SR_DirectXShaderCompiler.h"
#include "RenderCore/Resources/SR_TextureLoading.h"

SC_Ref<SGfx_MaterialInstance> SGfx_MaterialInstance::GetDefault()
{
	static SC_Ref<SGfx_MaterialInstance> instance = nullptr;
	if (!instance)
	{
		SGfx_MaterialProperties materialProps;
		materialProps.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Grey_1x1.dds");
		materialProps.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Black_1x1.dds");
		materialProps.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Normal_1x1.dds");
		materialProps.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Grey_1x1.dds");
		materialProps.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_White_1x1.dds");
		materialProps.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_White_1x1.dds");

		materialProps.mUseAlphaTesting = false;
		materialProps.mOutputVelocity = true;
		materialProps.mShaderProperties.mRasterizerProperties.mCullMode = SR_CullMode::Back;
		materialProps.mShaderProperties.mBlendStateProperties.mNumRenderTargets = 1;
		materialProps.mShaderProperties.mRTVFormats.mNumColorFormats = 1;
		materialProps.mShaderProperties.mRTVFormats.mColorFormats[0] = SR_Format::RGBA8_UNORM;
		materialProps.mShaderProperties.mDepthStencilProperties.mWriteDepth = false;
		materialProps.mShaderProperties.mDepthStencilProperties.mDepthComparisonFunc = SR_ComparisonFunc::Equal;

		SR_ShaderCompileArgs args;
		args.mEntryPoint = "Main";
		args.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/PixelShaderDefault.ssf";
		args.mType = SR_ShaderType::Pixel;
		SR_RenderDevice::gInstance->CompileShader(args, materialProps.mShaderProperties.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Pixel)]);

		SC_Ref<SGfx_Material> materialTemplate = SC_MakeRef<SGfx_Material>(materialProps);
		instance = SC_MakeRef<SGfx_MaterialInstance>(materialTemplate);
	}
    return instance;
}

SC_Ref<SGfx_MaterialInstance> SGfx_MaterialInstance::Create(const SC_Ref<SGfx_Material>& aMaterialTemplate)
{
	return SC_MakeRef<SGfx_MaterialInstance>(aMaterialTemplate);
}

SGfx_MaterialInstance::SGfx_MaterialInstance(const SC_Ref<SGfx_Material>& aMaterialTemplate)
    : mMaterialTemplate(aMaterialTemplate)
	, mMaterialIndex(SC_UINT32_MAX)
{
	mTextureOverrides.Add(mMaterialTemplate->GetTextures());
	UpdateGPUData();
}

SGfx_MaterialInstance::~SGfx_MaterialInstance()
{
	SGfx_MaterialGPUDataBuffer::Get().Remove(mMaterialIndex);
}

SGfx_Material* SGfx_MaterialInstance::GetMaterialTemplate() const
{
    return mMaterialTemplate;
}

uint32 SGfx_MaterialInstance::GetMaterialIndex() const
{
	return mMaterialIndex;
}

void SGfx_MaterialInstance::OverrideTexture(const SC_Ref<SR_Texture>& aTexture, uint32 aIndex)
{
	SC_ASSERT(aIndex < mTextureOverrides.Count());
	mTextureOverrides[aIndex] = aTexture;
	UpdateGPUData();
}

void SGfx_MaterialInstance::UpdateGPUData()
{
	SGfx_MaterialGPUData gpuData;

	uint32 i = 0;
	for (const SC_Ref<SR_Texture>& tex : mTextureOverrides)
	{
		gpuData.mTextureIndices[i] = tex->GetDescriptorHeapIndex();
		++i;
	}

	if (mMaterialIndex == SC_UINT32_MAX)
		mMaterialIndex = SGfx_MaterialGPUDataBuffer::Get().Add(gpuData);
	else
		SGfx_MaterialGPUDataBuffer::Get().UpdateData(gpuData, mMaterialIndex);
}
