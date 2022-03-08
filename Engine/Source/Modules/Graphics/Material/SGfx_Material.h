#pragma once
#include "RenderCore/Interface/SR_ShaderState.h"

class SR_Texture;

struct SGfx_MaterialProperties
{
	SR_ShaderStateProperties mShaderProperties;
	SC_Array<SC_FilePath> mTextures;

	float mAlphaRef;
	bool mUseAlphaTesting;
	bool mOutputVelocity;
};

enum class SGfx_MaterialShadingModel
{
	Flat,
	Default,
	Subsurface,
	ClearCoat,
	Hair,
	Eye,
	Cloth,

	COUNT
};

enum class SGfx_MaterialAttributes
{
	BaseColor,
	Normal,
	Roughness,
	Metallic,
	AmbientOcclusion,
	Specular,
	Opacity,
	ShadingModel,

	COUNT
};

enum class SGfx_MaterialType
{
	Surface,
	Decal,
	Volume,
	PostProcess,

	COUNT
};

enum class SGfx_MaterialBlendMode
{
	Opaque,
	AlphaMasked,
	Translucent,
	Additive,

	COUNT
};

enum class SGfx_MaterialShaderType
{
	Default,
	Depth,
	ShadowDepth,

	COUNT
};

template<class T>
struct SGfx_MaterialParameter
{
public:
	SGfx_MaterialParameter(const T& aParameter) : mParameter(aParameter) {}

	T& Get() { return mParameter; }
	const T& Get() const { return mParameter; }

private:
	T mParameter;
};

class SGfx_MaterialInput
{
public:
	SGfx_MaterialInput();
	virtual ~SGfx_MaterialInput();

	bool Compile();
protected:
};

static constexpr uint32 SGfx_MaxMaterialTextureCount = 16;
class SGfx_Material
{
public:
	SGfx_Material();
	SGfx_Material(const SGfx_MaterialProperties& aProperties);
	~SGfx_Material();

	bool Init(const SGfx_MaterialProperties& aProperties);

	SR_ShaderState* GetShaderState(const SR_VertexLayout& aVertexLayout, SGfx_MaterialShaderType aType = SGfx_MaterialShaderType::Default);
	const SC_Array<SC_Ref<SR_Texture>>& GetTextures() const;

	bool SaveToFile(const SC_FilePath& aFilePath) const;
	bool LoadFromFile(const SC_FilePath& aFilePath);

	float GetAlphaRef() const { return mAlphaRef; }
	bool UseAlphaTesting() const { return mUseAlphaTesting; }
	bool OutputVelocity() const { return mOutputVelocity; }

	void SetMaterialType(const SGfx_MaterialType& aType);
	const SGfx_MaterialType GetMaterialType() const;

	void SetShadingModel(const SGfx_MaterialShadingModel& aShadingModel);
	const SGfx_MaterialShadingModel& GetShadingModel() const;

	void SetBlendMode(const SGfx_MaterialBlendMode& aBlendMode);
	const SGfx_MaterialBlendMode& GetBlendMode() const;

	const SC_FilePath& GetSourceFile() const { return mSourceFile; }
private:

	void AddTexture(SC_Ref<SR_Texture> aTexture);
	SC_Array<SC_Ref<SR_Texture>> mTextures;

	SC_UnorderedMap<SR_VertexLayout, SC_Ref<SR_ShaderState>> mShaderStates[static_cast<uint32>(SGfx_MaterialShaderType::COUNT)];
	SR_ShaderCompileArgs mShaderCompileArgs[static_cast<uint32>(SR_ShaderType::COUNT)];
	SR_ShaderStateProperties mShaderStateProperties[static_cast<uint32>(SGfx_MaterialShaderType::COUNT)];

	mutable SC_FilePath mSourceFile;

	float mAlphaRef;
	bool mUseAlphaTesting;
	bool mOutputVelocity;

	SGfx_MaterialType mMaterialType;
	SGfx_MaterialShadingModel mShadingModel;
	SGfx_MaterialBlendMode mMaterialBlendMode;

	SGfx_MaterialInput mBaseColor;
	SGfx_MaterialInput mNormal;
	SGfx_MaterialInput mRoughness;
	SGfx_MaterialInput mMetallic;
	SGfx_MaterialInput mAmbientOcclusion;
	SGfx_MaterialInput mSpecular;
	SGfx_MaterialInput mOpacity;
	SGfx_MaterialInput mAlphaMask;

	bool mEnableGeometricSpecularAA;
	bool mEnableTangentSpaceNormals;
};


/*
	MATERIAL JSON TEMPLATE
	{
		"MainShader": "Shaders/DefaultMeshShader.ssf",
		"Properties": 
		[
			"MaterialType": 0,
			"ShadingModel": 1,
			"BlendMode": 0,
			"AlphaRef": 0.5f,
			"OutputVelocity": true
		]
	}
*/