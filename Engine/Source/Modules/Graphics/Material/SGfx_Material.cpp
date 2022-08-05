#include "SGfx_Material.h"
#include "Platform/Async/SC_Thread.h"

#include <queue>
#include <thread>
#include <fstream>
#include <sstream>

class SGfx_MaterialCompilerThread : public SC_Thread
{
public:
    static SGfx_MaterialCompilerThread& Get()
    {
        static SGfx_MaterialCompilerThread gInstance;
        return gInstance;
    }

	void Queue(std::function<void()> aJob)
	{
		SC_MutexLock lock(mMutex);
		mJobs.push(aJob);
		mHasWorkEvent.Signal();
	}

protected:
    SGfx_MaterialCompilerThread()
    {
        SetName("Material Compiler Thread");
        Start();
    }
	~SGfx_MaterialCompilerThread() 
    {
        Stop(false);
        mHasWorkEvent.Signal();
        Wait();
    }

    void ThreadMain() override
    {
        while (mIsRunning)
        {
            mHasWorkEvent.Wait();
            mHasWorkEvent.Reset();

			while (!mJobs.empty())
			{
				std::function<void()> job;
				{
					SC_MutexLock lock(mMutex);
					job = mJobs.front();
					mJobs.pop();
				}

				if (job)
					job();
			}
        }
    }

private:
    SC_Mutex mMutex;
    SC_Event mHasWorkEvent;
    std::queue<std::function<void()>> mJobs;
};


SGfx_Material::SGfx_Material()
{
}

SGfx_Material::SGfx_Material(const SGfx_MaterialProperties& aProperties)
{
	Init(aProperties);
}

SGfx_Material::~SGfx_Material()
{

}

bool SGfx_Material::Init(const SGfx_MaterialProperties& aProperties)
{
	mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::Default)] = aProperties.mShaderProperties;
	mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::Depth)] = aProperties.mShaderProperties;
	mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::Depth)].mDepthStencilProperties.mWriteDepth = true;
	mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::Depth)].mDepthStencilProperties.mDepthComparisonFunc = SR_ComparisonFunc::GreaterEqual;
	mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::ShadowDepth)] = mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::Depth)];
	mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::ShadowDepth)].mRasterizerProperties.mCullMode = SR_CullMode::Back;
	mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::ShadowDepth)].mRasterizerProperties.mDepthBias = -100;
	mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::ShadowDepth)].mRasterizerProperties.mSlopedScaleDepthBias = -2.0f;

	if (aProperties.mOutputVelocity)
		mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::Depth)].mRTVFormats.mColorFormats[0] = SR_Format::RG16_FLOAT;

	SR_ShaderCompileArgs args;
	args.mEntryPoint = "Main";
	args.mDefines.Add(SC_Pair<std::string, std::string>("PIXEL_SHADER", "1"));
	args.mType = SR_ShaderType::Pixel;

	std::string depthShaderCode;
	if (aProperties.mUseAlphaTesting)
	{
		depthShaderCode =
			"#include \"Common.ssh\" \n"
			"#include \"MaterialCommon.ssh\" \n"
			"#include \"MotionVector.ssh\" \n"
			"struct PixelShaderInput\n"
			"{\n"
			"   float4 mClipPosition : SV_POSITION;\n"
			"   float3 mViewPosition : View_POSITION;\n"
			"   float3 mWorldPosition : WORLD_POSITION;\n"
			"   float3 mPrevWorldPosition : PREV_WORLD_POSITION;\n"
			"   float3 mNormal : NORMAL0;\n"
			"   float3 mTangent : TANGENT;\n"
			"   float3 mBitangent : BITANGENT;"
			"   float2 mUV : UV0;\n"
			"   nointerpolation uint mMeshletIndex : MESHLET_INDEX0;\n"
			"   nointerpolation uint mMaterialIndex : MATERIAL_INDEX;\n"
			"};\n";

		if (aProperties.mOutputVelocity)
			depthShaderCode += "float2 Main(const PixelShaderInput aInput) : SV_TARGET0\n";
		else
			depthShaderCode += "void Main(const PixelShaderInput aInput)\n";

		depthShaderCode +=
			"{\n"
			"   SR_MaterialInfo materialInfo = SR_GetMaterialInfo(aInput.mMaterialIndex);\n"
			"   Texture2D<float> alphaMask = GetTexture2D<float>(materialInfo.mTextureIndices[4]);\n"
			"   if (alphaMask.Sample(gBilinearClamp, aInput.mUV).r < 0.5f)\n"
			"       discard;\n";

		if (aProperties.mOutputVelocity)
			depthShaderCode += "   return CalculateMotionVector(aInput.mWorldPosition, aInput.mPrevWorldPosition);\n";

		depthShaderCode += "}\n";
	}
	else
	{
		if (aProperties.mOutputVelocity)
		{
			depthShaderCode +=
				"#include \"MotionVector.ssh\" \n"
				"struct PixelShaderInput\n"
				"{\n"
				"   float4 mClipPosition : SV_POSITION;\n"
				"   float3 mViewPosition : View_POSITION;\n"
				"   float3 mWorldPosition : WORLD_POSITION;\n"
				"   float3 mPrevWorldPosition : PREV_WORLD_POSITION;\n"
				"   float3 mNormal : NORMAL0;\n"
				"   float3 mTangent : TANGENT;\n"
				"   float3 mBitangent : BITANGENT;"
				"   float2 mUV : UV0;\n"
				"   nointerpolation uint mMeshletIndex : MESHLET_INDEX0;\n"
				"   nointerpolation uint mMaterialIndex : MATERIAL_INDEX;\n"
				"};\n"
				"float2 Main(const PixelShaderInput aInput) : SV_TARGET0\n"
				"{\n"
				"   return CalculateMotionVector(aInput.mWorldPosition, aInput.mPrevWorldPosition);\n"
				"}\n";
		}
		else
			depthShaderCode = "void Main() {}";

	}
	SR_RenderDevice::gInstance->CompileShader(depthShaderCode, args, mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::Depth)].mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Pixel)]);

	mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::DefaultMeshlet)] = mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::Default)];
	mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::DepthMeshlet)] = mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::Depth)];
	mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::ShadowDepthMeshlet)] = mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::ShadowDepth)];

	mTextures.Reserve(aProperties.mTextures.Count());
	for (const SC_FilePath& path : aProperties.mTextures)
	{
		AddTexture(SR_RenderDevice::gInstance->LoadTexture(path));
	}

	mUseAlphaTesting = aProperties.mUseAlphaTesting;
	mAlphaRef = aProperties.mAlphaRef;
	mOutputVelocity = aProperties.mOutputVelocity;
    return true;
}

static const char* GetTypeFromFormat(const SR_Format& aFormat)
{
    switch (aFormat)
    {
    case SR_Format::RGBA32_FLOAT:
		return "float4";
	case SR_Format::RGB32_FLOAT:
		return "float3";
	case SR_Format::RG32_FLOAT:
		return "float2";
	case SR_Format::R32_FLOAT:
		return "float";
	case SR_Format::RGBA32_UINT:
		return "uint4";
	case SR_Format::RGB32_UINT:
		return "uint3";
	case SR_Format::RG32_UINT:
		return "uint2";
	case SR_Format::R32_UINT:
		return "uint";
    }

    return "";
}

static const char* locGetVertexAttributeSemantic(const SR_VertexAttribute& aAttributeId)
{
	switch (aAttributeId)
	{
	case SR_VertexAttribute::Position: return "POSITION";
	case SR_VertexAttribute::Normal: return "NORMAL";
	case SR_VertexAttribute::Tangent: return "TANGENT";
	case SR_VertexAttribute::Bitangent: return "BITANGENT";
	case SR_VertexAttribute::UV: return "UV";
	case SR_VertexAttribute::Color: return "COLOR";
	case SR_VertexAttribute::BoneId: return "BONEID";
	case SR_VertexAttribute::BoneWeight: return "BONEWEIGHT";
	}

	SC_ASSERT(false, "Unknown attribute");
	return "<unknown>";
}
static const char* locGetVertexAttributeVariableName(const SR_VertexAttribute& aAttributeId)
{
	switch (aAttributeId)
	{
	case SR_VertexAttribute::Position: return "mPosition";
	case SR_VertexAttribute::Normal: return "mNormal";
	case SR_VertexAttribute::Tangent: return "mTangent";
	case SR_VertexAttribute::Bitangent: return "mBitangent";
	case SR_VertexAttribute::UV: return "mUV";
	case SR_VertexAttribute::Color: return "mColor";
	case SR_VertexAttribute::BoneId: return "mBoneId";
	case SR_VertexAttribute::BoneWeight: return "mBoneWeight";
	}

	SC_ASSERT(false, "Unknown attribute");
	return "<unknown>";
}

SR_ShaderState* SGfx_Material::GetShaderState(const SR_VertexLayout& aVertexLayout, SGfx_MaterialShaderType aType)
{
    auto& shaderStates = mShaderStates[static_cast<uint32>(aType)];
    if (shaderStates.count(aVertexLayout) > 0)
        return shaderStates[aVertexLayout];
    else
    {
        // Create shader version for this vertex layout
        shaderStates.insert(std::make_pair(aVertexLayout, nullptr));

        // Launch async compile for this permutation
        auto CompileMaterial = [this, aType, aVertexLayout]()
        {
            const uint32 shaderType = static_cast<uint32>(aType);
			SR_ShaderStateProperties shaderProperties(mShaderStateProperties[shaderType]);
            shaderProperties.mVertexLayout = aVertexLayout;

			std::stringstream vertexLayoutCode;

			// Generate mesh code
			vertexLayoutCode << "struct SR_VertexLayout\n{\n";

			for (const SR_VertexAttributeData& vertexAttribute : aVertexLayout.mAttributes)
			{
				vertexLayoutCode << SC_FormatStr("{} {}{} : {}{};\n", 
					GetTypeFromFormat(vertexAttribute.mFormat),
					locGetVertexAttributeVariableName(vertexAttribute.mAttributeId),
					(vertexAttribute.mAttributeIndex) > 0 ? std::to_string(vertexAttribute.mAttributeIndex) : "",
					locGetVertexAttributeSemantic(vertexAttribute.mAttributeId),
					vertexAttribute.mAttributeIndex
				);

			}
			vertexLayoutCode << "};\n\n";

			std::string shaderCodeBuffer;

#if SR_ENABLE_MESH_SHADERS
            if (SR_RenderDevice::gInstance->GetSupportCaps().mEnableMeshShaders && SGfx_IsMeshletMaterialShaderType(aType))
			{
				std::ifstream t((SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/DefaultMeshShader.ssf").GetAbsolutePath());
				shaderCodeBuffer = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

				SR_ShaderCompileArgs meshShaderCompileArgs;
                meshShaderCompileArgs.mDefines.Add(SC_Pair<std::string, std::string>("MESH_SHADER", "1"));
                meshShaderCompileArgs.mEntryPoint = "MainMS";
                meshShaderCompileArgs.mType = SR_ShaderType::Mesh;

                std::string meshShaderCode(vertexLayoutCode.str() + shaderCodeBuffer);
                SR_RenderDevice::gInstance->CompileShader(meshShaderCode, meshShaderCompileArgs, shaderProperties.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Mesh)]);
            }
            else
#endif
			{
				std::ifstream t((SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/DefaultVertexShader.ssf").GetAbsolutePath());
				shaderCodeBuffer = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

				SR_ShaderCompileArgs vertexShaderCompileArgs;
                vertexShaderCompileArgs.mEntryPoint = "Main";
                vertexShaderCompileArgs.mType = SR_ShaderType::Vertex;

				std::string vertexShaderCode(vertexLayoutCode.str() + shaderCodeBuffer);
                SR_RenderDevice::gInstance->CompileShader(vertexShaderCode, vertexShaderCompileArgs, shaderProperties.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Vertex)]);
            }

			mShaderStates[shaderType][aVertexLayout] = SR_RenderDevice::gInstance->CreateShaderState(shaderProperties);
        };
        SGfx_MaterialCompilerThread::Get().Queue(CompileMaterial);

        return shaderStates[aVertexLayout];
    }
}

const SC_Array<SC_Ref<SR_Texture>>& SGfx_Material::GetTextures() const
{
    return mTextures;
}

bool SGfx_Material::SaveToFile(const SC_FilePath& aFilePath) const
{
    SC_Json saveData;

    saveData["MainShader"] = (SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/DefaultMeshShader.ssf").GetStr();

    for (const SC_Ref<SR_Texture>& tex : mTextures)
    {
        saveData["Textures"].push_back(tex->GetResource()->GetProperties().mSourceFile.GetStr());
    }

	{
		SC_Json& properties = saveData["Properties"];
		properties["MaterialType"] = static_cast<uint32>(mMaterialType);
		properties["ShadingModel"] = static_cast<uint32>(mShadingModel);
		properties["BlendMode"] = static_cast<uint32>(mMaterialBlendMode);
		properties["AlphaRef"] = mAlphaRef;
		properties["OutputVelocity"] = mOutputVelocity;
	}

	mSourceFile = aFilePath.GetStr();
    return SC_SaveJson(aFilePath, saveData);
}

bool SGfx_Material::LoadFromFile(const SC_FilePath& aFilePath)
{
	SC_Json savedData;
    if (!SC_LoadJson(aFilePath, savedData))
        return false;

	if (savedData.contains("Properties"))
	{
		const SC_Json& properties = savedData["Properties"];

		mMaterialType = static_cast<SGfx_MaterialType>(properties["MaterialType"].get<uint32>());
		mShadingModel = static_cast<SGfx_MaterialShadingModel>(properties["ShadingModel"].get<uint32>());
		mMaterialBlendMode = static_cast<SGfx_MaterialBlendMode>(properties["BlendMode"].get<uint32>());
		mAlphaRef = properties["AlphaRef"].get<float>();
		mOutputVelocity = properties["OutputVelocity"].get<bool>();
	}

    mSourceFile = aFilePath.GetStr();
    return true;
}

void SGfx_Material::SetMaterialType(const SGfx_MaterialType& aType)
{
	mMaterialType = aType;
}

const SGfx_MaterialType SGfx_Material::GetMaterialType() const
{
	return mMaterialType;
}

void SGfx_Material::SetShadingModel(const SGfx_MaterialShadingModel& aShadingModel)
{
	mShadingModel = aShadingModel;
}

const SGfx_MaterialShadingModel& SGfx_Material::GetShadingModel() const
{
	return mShadingModel;
}

void SGfx_Material::SetBlendMode(const SGfx_MaterialBlendMode& aBlendMode)
{
	mMaterialBlendMode = aBlendMode;
}

const SGfx_MaterialBlendMode& SGfx_Material::GetBlendMode() const
{
	return mMaterialBlendMode;
}

void SGfx_Material::AddTexture(SC_Ref<SR_Texture> aTexture)
{
	SC_ASSERT(mTextures.Count() < SGfx_MaxMaterialTextureCount);
	mTextures.Add(aTexture);
}

SGfx_MaterialInput::SGfx_MaterialInput()
{

}

SGfx_MaterialInput::~SGfx_MaterialInput()
{

}

bool SGfx_MaterialInput::Compile()
{
	return true;
}
