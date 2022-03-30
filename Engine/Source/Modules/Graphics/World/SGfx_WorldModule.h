#pragma once
#include "Common/Module/SC_Module.h"

class SGfx_WorldModule : public SC_Module
{
	SC_MODULE_DECLARE(SGfx_WorldModule, "Graphics.World", "");
	//SC_MODULE_DEPENDENCIES(Graphics.World, Graphics.Lights);
public:
	SGfx_WorldModule();
	~SGfx_WorldModule();
};

