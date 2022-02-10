#pragma once

struct SR_ShaderGenerationProperties;
struct SR_ShaderVariableProperties;

class SGfx_MeshShaderGenerator
{
public:
	SGfx_MeshShaderGenerator();
	~SGfx_MeshShaderGenerator();

	void Generate(SR_ShaderGenerationProperties& aOutGenerationProps);

private:
	enum class Semantic
	{
		ClipPosition,
		WorldPosition,
		ViewPosition,
		WorldNormal,
		ModelPosition,
		ModelNormal,
		UV,
		Color,
		ModelTangent,
		ModelBitangent,
		TangentToWorld,

		AlphaRef
	};

	struct InputData
	{
		Semantic mSemantic;
		std::string mType;
		std::string mName;
	};

	enum class InOutGenerationSemantic
	{
		WorldPosition,
		ClipPosition,
		InstancePosition,
		WorldNormal,
		Color,
		UV0,
		UV1,
		UV2,
		UV3,
		Tangent,
		COUNT
	};

	void GenerateVertexShader(SR_ShaderGenerationProperties& aOutGenerationProps);
	void GenerateMeshShader(SR_ShaderGenerationProperties& aOutGenerationProps);
	void GeneratePixelShader(SR_ShaderGenerationProperties& aOutGenerationProps);

	void GenerateVertexShaderInputs(SC_Array<SR_ShaderVariableProperties>& aInputs);
	void GenerateMeshShaderInputs(SC_Array<SR_ShaderVariableProperties>& aInputs);
	void GenerateVertexOutputs(SC_Array<SR_ShaderVariableProperties>& aOutputs);

	bool GenerateInstanceDataCode(SR_ShaderGenerationProperties& aOutGenerationProps);

private:

	SR_VertexLayout mVertexLayout;

	static uint32 gVersion;
};