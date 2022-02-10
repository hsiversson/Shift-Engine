#pragma once
#include "SGfx_MaterialNode.h"

class SGfx_MaterialNode_TextureSample : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_TextureSample();
	bool Compile(SGfx_MaterialCompiler* aCompiler) override;
public:
	enum InputPinIndex
	{
		InTexture,
		InSampler,
		InUV,
		InMip
	};
	enum OutputPinIndex
	{
		OutRGB,
		OutR,
		OutG,
		OutB,
		OutA,
	};
};

