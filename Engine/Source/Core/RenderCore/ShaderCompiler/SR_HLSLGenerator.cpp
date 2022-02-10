#include "SR_HLSLGenerator.h"

static constexpr const char* gHLSLHeader =
	"#pragma warning(error: 3206)\n"	// implicit truncation of vector type
	"#pragma warning(error: 3556)\n"	// integer divides may be much slower, try using uints if possible
	"#pragma warning(error: 1519)\n"	// macro redefinition
	
	"#define IS_HLSL (1)"

	"float2 SR_MakeVec(float aValue0, float aValue1) { return float2(aValue0, aValue1); }\n"
	"float3 SR_MakeVec(float2 aValue0, float aValue1) { return float3(aValue0, aValue1); }\n"
	"float4 SR_MakeVec(float3 aValue0, float aValue1) { return float4(aValue0, aValue1); }\n"
	"int2 SR_MakeVec(int aValue0, int aValue1) { return int2(aValue0, aValue1); }\n"
	"int3 SR_MakeVec(int2 aValue0, int aValue1) { return int3(aValue0, aValue1); }\n"
	"int4 SR_MakeVec(int3 aValue0, int aValue1) { return int4(aValue0, aValue1); }\n"

	"#define SR_Sample(aName, aSampler, aUV) aName.Sample(aSampler, aUV)\n"
	"#define SR_SampleGrad(aName, aSampler, aUV, aDx, aDy) aName.SampleGrad(aSampler, aUV, aDx, aDy)\n"
	"#define SR_SampleMip(aName, aSampler, aUV, aMip) aName.SampleLevel(aSampler, aUV, aMip)\n"
	"#define SR_SampleMip0(aName, aSampler, aUV) aName.SampleLevel(aSampler, aUV, 0)\n"
	"#define SR_SampleCmp(aName, aSampler, aUV, aValue) aName.SampleCmp(aSampler, aUV, aValue)\n"
	"#define SR_SampleCmpMip0(aName, aSampler, aUV, aValue) aName.SampleCmpLevelZero(aSampler, aUV, aValue)\n"
	"#define SR_SampleTexelMip(aName, aSampler, aPos, aMip) aName.Load(SR_MakeVec(aPos, aMip))\n"

	"#define SR_GetMip(aName, aSampler, aUV) aName.CalculateLevelOfDetail(aSampler, aUV)\n"

	"#define SR_Load(aName, aAddress) aName.Load(aAddress)\n"
	"#define SR_Load2(aName, aAddress) aName.Load2(aAddress)\n"
	"#define SR_Load3(aName, aAddress) aName.Load3(aAddress)\n"
	"#define SR_Load4(aName, aAddress) aName.Load4(aAddress)\n"
	"#define SR_Store(aName, aAddress, aValue) aName.Store(aAddress, aValue)\n"
	"#define SR_Store2(aName, aAddress, aValue) aName.Store2(aAddress, aValue)\n"
	"#define SR_Store3(aName, aAddress, aValue) aName.Store3(aAddress, aValue)\n"
	"#define SR_Store4(aName, aAddress, aValue) aName.Store4(aAddress, aValue)\n"

	"#define SR_AtomicAdd InterlockedAdd\n"
	"#define SR_AtomicExchange InterlockedExchange\n"
	"#define SR_AtomicMin InterlockedMin\n"
	"#define SR_AtomicMax InterlockedMax\n"
	"#define SR_AtomicAnd InterlockedAnd\n"
	"#define SR_AtomicOr InterlockedOr\n"
	"#define SR_AtomicXor InterlockedXor\n"

	"#define SR_MemoryBarrier AllMemoryBarrier\n"
	"#define SR_MemoryBarrierSynced AllMemoryBarrierWithGroupSync\n"
	"#define SR_SharedMemoryBarrier DeviceMemoryBarrier\n"
	"#define SR_SharedMemoryBarrierSynced DeviceMemoryBarrierWithGroupSync\n"
	"#define SR_GroupMemoryBarrier GroupMemoryBarrier\n"
	"#define SR_GroupMemoryBarrierSynced GroupMemoryBarrierWithGroupSync\n"

	"#define SR_Branch [branch]\n"
	"#define SR_Flatten [flatten]\n"
	"#define SR_Loop [loop]\n"
	"#define SR_Unroll [unroll]\n"
	"#define SR_UnrollN(aNum) [unroll(aNum)]\n"

	"float2 SR_Transform(float2x2 aMat, float2 aVec) { return mul(aMat, aVec); }\n"
	"float3 SR_Transform(float3x3 aMat, float3 aVec) { return mul(aMat, aVec); }\n"
	"float3 SR_Transform(float3x4 aMat, float3 aVec) { return mul(aMat, float4(aVec, 1.0f)); }\n"
	"float3 SR_Transform(float4x3 aMat, float3 aVec) { return mul(aMat, aVec); }\n"
	"float4 SR_Transform(float3x4 aMat, float4 aVec) { return mul(aMat, aVec); }\n"
	"float4 SR_Transform(float4x4 aMat, float4 aVec) { return mul(aMat, aVec); }\n"
	"float4 SR_Transform(float4x4 aMat, float3 aVec) { return mul(aMat, float4(aVec, 1.0f)); }\n"
	"float4 SR_TransformNormal(float4x4 aMat, float3 aVec) { return mul((float3x3)aMat, aVec); }\n"

	;

static constexpr const char* gHalfPrecisionHeaderPC =
	"#define half min16float\n"
	"#define half1 min16float1\n"
	"#define half2 min16float2\n"
	"#define half3 min16float3\n"
	"#define half4 min16float4\n"
	;

SR_HLSLGenerator::SR_HLSLGenerator()
{

}

SR_HLSLGenerator::~SR_HLSLGenerator()
{

}

bool SR_HLSLGenerator::GenerateHLSL(const SR_ShaderGenerationProperties& aShaderProperties, std::string& aOutShaderCode)
{
	aOutShaderCode.clear();
	
	SR_API api = SR_API::D3D12;
	bool isPC = true;

	switch (api)
	{
	case SR_API::D3D12:

		aOutShaderCode += gHLSLHeader;
		if (isPC)
		{
			aOutShaderCode += gHalfPrecisionHeaderPC;
		}

		break;
	default:
		return false;
	}
	
	//Handle includes

	if (aShaderProperties.mShaderType != SR_ShaderType::Compute)
	{
		if (!BuildInputStruct(aShaderProperties.mInputs, aOutShaderCode))
			return false;
	}

	bool hasOutputs = !aShaderProperties.mOutputs.IsEmpty();
	if (hasOutputs)
	{
		if (!BuildOutputStruct(aShaderProperties.mOutputs, aOutShaderCode))
			return false;
	}

	// Declarations

	// RT HitGroup functions
	// RT SubShader

	// Shader specific params

#if ENABLE_RAYTRACING
	if (aShaderProperties.mShaderType == SR_ShaderType::Raytracing)
		aOutShaderCode += "[shader(\"raygeneration\")]\n";
#endif

	aOutShaderCode += hasOutputs ? "OutputStruct main(" : "void main(";
	if (!InsertMainArgs(aShaderProperties.mMainFuncArgs, aOutShaderCode))
		return false;

	aOutShaderCode += ")\n{\n";

	if (hasOutputs)
		aOutShaderCode += "OutputStruct output;\n";

	aOutShaderCode += aShaderProperties.mMainCode;


	if (hasOutputs)
		aOutShaderCode += "return output;\n";

	aOutShaderCode += "}\n";

	return true;
}

bool SR_HLSLGenerator::ResolveSemantic(const SR_ShaderVariableProperties& aVariableProps, std::string& aOutLine) const
{
	SR_Semantic::Semantic semantic = aVariableProps.mSemantic;
	switch (semantic)
	{
	case SR_Semantic::SR_Position:
		aOutLine = "SV_Position";
		return true;
	case SR_Semantic::SR_Depth:
		aOutLine = "SV_Depth";
		return true;
	case SR_Semantic::SR_PixelCoord:
		aOutLine = "SV_Position";
		return true;
	case SR_Semantic::SR_Target:
		aOutLine = "SV_Target";
		aOutLine += std::to_string(SC_Max(0, aVariableProps.mSemanticIndex));
		return true;
	case SR_Semantic::SR_FrontFace:
		aOutLine = "SV_IsFrontFace";
		return true;
	case SR_Semantic::SR_VertexId:
		aOutLine = "SV_VertexID";
		return true;
	case SR_Semantic::SR_DispatchThreadId:
		aOutLine = "SV_DispatchThreadID";
		return true;
	case SR_Semantic::SR_GroupThreadId:
		aOutLine = "SV_GroupThreadID";
		return true;
	case SR_Semantic::SR_GroupId:
		aOutLine = "SV_GroupID";
		return true;
	case SR_Semantic::SR_GroupIndex:
		aOutLine = "SV_GroupIndex";
		return true;
	default:
		return false;
	}
}

bool SR_HLSLGenerator::BuildVariable(const SR_ShaderVariableProperties& aVariableProps, std::string& aOutLine) const
{
	if ((aVariableProps.mFlags & SR_ShaderVariableFlag_InOut) == SR_ShaderVariableFlag_InOut)
		aOutLine += "inout ";
	else if (aVariableProps.mFlags & SR_ShaderVariableFlag_In)
		aOutLine += "in ";
	else if (aVariableProps.mFlags & SR_ShaderVariableFlag_Out)
		aOutLine += "out ";

	if (aVariableProps.mFlags & SR_ShaderVariableFlag_Centroid)
		aOutLine += "centroid ";
	else if (aVariableProps.mFlags & SR_ShaderVariableFlag_NoInterpolation)
		aOutLine += "nointerpolation ";
	else if (aVariableProps.mFlags & SR_ShaderVariableFlag_Precise)
		aOutLine += "precise ";
	else if (aVariableProps.mFlags & SR_ShaderVariableFlag_LDS)
		aOutLine += "groupshared ";

	aOutLine += aVariableProps.mType;
	aOutLine += " ";
	aOutLine += aVariableProps.mName;

	return true;
}

bool SR_HLSLGenerator::BuildInputStruct(const SC_Array<SR_ShaderVariableProperties>& /*aInputs*/, std::string& /*aOutCode*/)
{
	return false;
}

bool SR_HLSLGenerator::BuildOutputStruct(const SC_Array<SR_ShaderVariableProperties>& /*aOutputs*/, std::string& /*aOutCode*/)
{
	return false;
}

bool SR_HLSLGenerator::InsertMainArgs(const SC_Array<SR_ShaderVariableProperties>& /*aArgs*/, std::string& /*aOutCode*/)
{
	return false;
}
