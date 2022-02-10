#include "SGfx_MaterialCompiler.h"

SGfx_MaterialCompiler::SGfx_MaterialCompiler()
{

}

SGfx_MaterialCompiler::~SGfx_MaterialCompiler()
{

}

void SGfx_MaterialCompiler::AddCode(const char* aCodeStr)
{
	code += aCodeStr;
}

void SGfx_MaterialCompiler::AddCode(const std::string& aCodeStr)
{
	code += aCodeStr;
}

bool SGfx_MaterialCompiler::Compile(SR_ShaderGenerationProperties& aGenerationProperties)
{
	SR_ShaderVariableProperties& outputVariable = aGenerationProperties.mOutputs.Add();
	outputVariable.mName = "mColor";
	outputVariable.mType = "float4";
	outputVariable.mSemantic = SR_Semantic::SR_Target;
	outputVariable.mSemanticIndex = 0;

	aGenerationProperties.mMainCode += code;

	aGenerationProperties.mMainCode += SC_FormatStr(
		"output.%s = float4(CalculateLighting(materialData, aInput.mWorldPosition, viewDirection, aInput.mViewPosition.z), 1.0f);\n",
		outputVariable.mName.c_str()
	);

	return true;
}
