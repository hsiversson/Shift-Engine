#include "SGfx_MeshShaderGenerator.h"
#include "RenderCore/ShaderCompiler/SR_ShaderGenerationProperties.h"

uint32 SGfx_MeshShaderGenerator::gVersion = 1;

SGfx_MeshShaderGenerator::SGfx_MeshShaderGenerator()
{

}

SGfx_MeshShaderGenerator::~SGfx_MeshShaderGenerator()
{

}

void SGfx_MeshShaderGenerator::Generate(SR_ShaderGenerationProperties& aOutGenerationProps)
{
	if (SR_RenderDevice::gInstance->GetSupportCaps().mEnableMeshShaders)
		GenerateMeshShader(aOutGenerationProps);
	else
		GenerateVertexShader(aOutGenerationProps);

	GeneratePixelShader(aOutGenerationProps);

}

void SGfx_MeshShaderGenerator::GenerateVertexShader(SR_ShaderGenerationProperties& aOutGenerationProps)
{
	GenerateVertexShaderInputs(aOutGenerationProps.mInputs);
	GenerateVertexOutputs(aOutGenerationProps.mOutputs);
}

void SGfx_MeshShaderGenerator::GenerateMeshShader(SR_ShaderGenerationProperties& aOutGenerationProps)
{
	GenerateMeshShaderInputs(aOutGenerationProps.mInputs);
	GenerateVertexOutputs(aOutGenerationProps.mOutputs);

	//if (needsClipPos)
	{
		aOutGenerationProps.mDeclarations.mIncludes.Add("SceneConstants.ssh");

		aOutGenerationProps.mMainCode += "\tclipPosition = SR_Transform(gSceneConstants.mViewConstants.mWorldToClip, worldPosition);\n";

	}

}

void SGfx_MeshShaderGenerator::GeneratePixelShader(SR_ShaderGenerationProperties& aOutGenerationProps)
{
	GenerateVertexOutputs(aOutGenerationProps.mInputs);
}

bool SGfx_MeshShaderGenerator::GenerateInstanceDataCode(SR_ShaderGenerationProperties& aOutGenerationProps)
{
	SR_ShaderFunctionDeclaration& funcDecl = aOutGenerationProps.mDeclarations.myFunctions.Add();
	funcDecl.mReturnType = "float4";
	funcDecl.mFunctionName = "LoadInstanceData";

	SR_ShaderVariableProperties& arg0 = funcDecl.mArguments.Add();
	arg0.mName = "aIndex";
	arg0.mType = "uint";

	funcDecl.mCode = 
		"\tStructuredBuffer<float4> instanceDataBuffer = ResourceDescriptorHeap[gSceneConstants.mInstanceDataBufferIndex];\n"
		"\treturn instanceDataBuffer[aIndex];\n";

	for (uint32 i = 0; i < 1; ++i)
	{
		InOutGenerationSemantic sem = InOutGenerationSemantic::ClipPosition;
		switch (sem)
		{
		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::WorldPosition:
			funcDecl.mCode += "\taOutput.SR_World_Position = SR_World_Position;\n";
			break;
		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::ClipPosition:
			funcDecl.mCode += "\taOutput.SR_Clip_Position = SR_Clip_Position;\n";
			break;
		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::InstancePosition:
			funcDecl.mCode += "\taOutput.SR_Instance_Position = SR_Instance_Position;\n";
			break;
		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::WorldNormal:
			funcDecl.mCode += "\taOutput.SR_World_Normal = SR_World_Normal;\n";
			break;
		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::Color:
			funcDecl.mCode += "\taOutput.SR_Color = SR_Color;\n";
			break;
		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::UV0:
			funcDecl.mCode += "\taOutput.SR_UV0 = SR_UV0;\n";
			break;
		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::UV1:
			funcDecl.mCode += "\taOutput.SR_UV1 = SR_UV1;\n";
			break;
		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::UV2:
			funcDecl.mCode += "\taOutput.SR_UV2 = SR_UV2;\n";
			break;
		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::UV3:
			funcDecl.mCode += "\taOutput.SR_UV3 = SR_UV3;\n";
			break;
		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::Tangent:
			funcDecl.mCode += "\taOutput.SR_Tangent = SR_Tangent;\n";
			break;
		default:
			break;
		}
	}

	return true;
}

void SGfx_MeshShaderGenerator::GenerateVertexShaderInputs(SC_Array<SR_ShaderVariableProperties>& /*aInputs*/)
{
}

void SGfx_MeshShaderGenerator::GenerateMeshShaderInputs(SC_Array<SR_ShaderVariableProperties>& /*aInputs*/)
{
}

void SGfx_MeshShaderGenerator::GenerateVertexOutputs(SC_Array<SR_ShaderVariableProperties>& aOutputs)
{
	for (uint32 i = 0; i < 1; ++i)
	{
		SR_ShaderVariableProperties& outVar = aOutputs.Add();
		InOutGenerationSemantic sem = InOutGenerationSemantic::ClipPosition;
		switch (sem)
		{
		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::WorldPosition:
			outVar.mType = "float3";
			outVar.mName = "SR_World_Position";
			break;

		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::ClipPosition:
			outVar.mType = "float4";
			outVar.mName = "SR_Clip_Position";
			break;

		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::InstancePosition:
			outVar.mType = "float3";
			outVar.mName = "SR_Instance_Position";
			outVar.mFlags = SR_ShaderVariableFlag_NoInterpolation;
			break;

		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::WorldNormal:
			outVar.mType = "float3";
			outVar.mName = "SR_World_Normal";
			break;

		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::Color:
			outVar.mType = "float4";
			outVar.mName = "SR_Color";
			break;

		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::UV0:
			outVar.mType = "float2";
			outVar.mName = "SR_UV0";
			break;

		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::UV1:
			outVar.mType = "float2";
			outVar.mName = "SR_UV1";
			break;

		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::UV2:
			outVar.mType = "float2";
			outVar.mName = "SR_UV2";
			break;

		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::UV3:
			outVar.mType = "float2";
			outVar.mName = "SR_UV3";
			break;

		case SGfx_MeshShaderGenerator::InOutGenerationSemantic::Tangent:
			outVar.mType = "float3";
			outVar.mName = "SR_Tangent";
			break;

		default:
			break;
		}
	}
}

