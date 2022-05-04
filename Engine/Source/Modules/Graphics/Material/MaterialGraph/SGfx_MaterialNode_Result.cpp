#include "SGfx_MaterialNode_Result.h"
#include "SGfx_MaterialCompiler.h"

SGfx_MaterialNode_Result::SGfx_MaterialNode_Result()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
}

SGfx_MaterialNode_Result::~SGfx_MaterialNode_Result()
{

}

bool SGfx_MaterialNode_Result::Compile(SGfx_MaterialCompiler* aCompiler)
{
	std::string finalStr;

	const SR_ShaderVariableProperties* inBaseColor			= mInputs[BaseColor].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* inNormal				= mInputs[Normal].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* inRoughness			= mInputs[Roughness].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* inMetallic			= mInputs[Metallic].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* inAmbientOcclusion	= mInputs[AmbientOcclusion].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* inSpecular			= mInputs[Specular].Read<SR_ShaderVariableProperties>();

	std::string code = SC_FormatStr(
		"float3 baseColor = {};\n"
		"float3 normal = {};\n"
		"float roughness = {};\n"
		"float metallic = {};\n"
		"float ambientOcclusion = {};\n"
		"float specular = {};\n",
		inBaseColor->mName.c_str(),
		inNormal->mName.c_str(),
		inRoughness->mName.c_str(),
		inMetallic->mName.c_str(),
		inAmbientOcclusion->mName.c_str(),
		inSpecular->mName.c_str()
	);

	bool convertNormalRange = true;
	if (convertNormalRange)
	{
		code += SC_FormatStr(
			"normal = normalize(normal * 2.0f + 1.0f);\n"
		);
	}

	bool hasTangentSpaceNormal = true;
	if (hasTangentSpaceNormal)
	{
		code += SC_FormatStr(
			"float3x3 tangentToWorld = float3x3(\n"
			"	aInput.mTangent.x, aInput.mBitangent.x, aInput.mNormal.x,\n"
			"	aInput.mTangent.y, aInput.mBitangent.y, aInput.mNormal.y,\n"
			"	aInput.mTangent.z, aInput.mBitangent.z, aInput.mNormal.z\n"
			"); \n"
			"normal = mul(tangentToWorld, normal);\n"
		);
	}

	code += SC_FormatStr(
		"MaterialData materialData = BuildMaterialData(float4(baseColor, 1.0f), normal, float4(roughness, metallic, ambientOcclusion, specular));\n"
	);

	aCompiler->AddCode(code);

	return true;
}
