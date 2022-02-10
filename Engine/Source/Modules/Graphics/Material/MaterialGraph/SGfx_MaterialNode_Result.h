#pragma once
#include "SGfx_MaterialNode.h"

class SGfx_MaterialCompiler;
class SGfx_MaterialNode_Result : public SGfx_MaterialNode
{
public:
	SGfx_MaterialNode_Result();
	~SGfx_MaterialNode_Result();

	bool Compile(SGfx_MaterialCompiler* aCompiler) override;

public:
	enum InputPinIndex
	{
		BaseColor,
		Normal,
		Roughness,
		Metallic,
		AmbientOcclusion,
		Specular,
		Opacity,
		ShadingModel
	};

private:
};

