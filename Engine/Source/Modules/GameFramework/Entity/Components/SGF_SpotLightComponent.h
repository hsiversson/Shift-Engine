#pragma once
#include "SGF_Component.h"

class SGfx_Light;

class SGF_SpotLightComponent : public SGF_Component 
{
	COMPONENT_DECLARE(SGF_SpotLightComponent, "SpotLight");

public:
	SGF_SpotLightComponent();
	~SGF_SpotLightComponent();

	//PROPERTY(float, mIntensity, SGF_PropertyAccess::Edit);

private:
	SC_Ref<SGfx_Light> mSpotLight;
};