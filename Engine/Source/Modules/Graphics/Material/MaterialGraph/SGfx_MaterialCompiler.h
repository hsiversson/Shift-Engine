#pragma once
#include "RenderCore/ShaderCompiler/SR_ShaderGenerationProperties.h"

class SGfx_MaterialCompiler
{
public:
	SGfx_MaterialCompiler();
	~SGfx_MaterialCompiler();

	void AddCode(const char* aCodeStr);
	void AddCode(const std::string& aCodeStr);

	void AddTextureResource();
	void AddRWTextureResource();

	void AddBufferResource();
	void AddRWBufferResource();

	bool Compile(SR_ShaderGenerationProperties& aGenerationProperties);

private:

private:
	SR_ShaderGenerationProperties mGenerationProperties;

	std::string code;
};

