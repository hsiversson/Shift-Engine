#pragma once
#include "SR_ShaderGenerationProperties.h"

class SR_ShaderParser
{
public:
	SR_ShaderParser();
	~SR_ShaderParser();

	bool ParseFile(const SC_FilePath& aFilePath);
	bool ParseString(const std::string& aString);

	const SR_ShaderGenerationProperties& GetShaderGenerationProperties() const;

private:

	bool Parse(const char*& aString, uint32& aCharsRemaining);
	bool ProcessGeneric(const char*& aString, uint32& aCharsRemaining);

	bool PreProcess(const char*& aString, uint32& aCharsRemaining);
	bool ProcessIncludes(const char*& aString, uint32& aCharsRemaining);
	bool ProcessComments(const char*& aString, uint32& aCharsRemaining);

	bool ProcessResourceBindings(const char*& aString, uint32& aCharsRemaining);

	bool ProcessComputeShaderProperties(const char*& aString, uint32& aCharsRemaining);

	bool IgnoreSpace(const char*& aString, uint32& aCharsRemaining);

private:
	bool PostError();
	bool PostWarning();

private:
	SR_ShaderGenerationProperties mShaderProps;
};

