#pragma once
#include "Defines/SR_Semantics.h"
#include "SR_ShaderGenerationProperties.h"

class SR_HLSLGenerator
{
public:
	SR_HLSLGenerator();
	~SR_HLSLGenerator();

	bool GenerateHLSL(const SR_ShaderGenerationProperties& aShaderProperties, std::string& aOutShaderCode);
private:

	bool ResolveSemantic(const SR_ShaderVariableProperties& aVariableProps, std::string& aOutLine) const;

	bool BuildVariable(const SR_ShaderVariableProperties& aVariableProps, std::string& aOutLine) const;

	bool BuildInputStruct(const SC_Array<SR_ShaderVariableProperties>& aInputs, std::string& aOutCode);
	bool BuildOutputStruct(const SC_Array<SR_ShaderVariableProperties>& aOutputs, std::string& aOutCode);
	bool InsertMainArgs(const SC_Array<SR_ShaderVariableProperties>& aArgs, std::string& aOutCode);
};

