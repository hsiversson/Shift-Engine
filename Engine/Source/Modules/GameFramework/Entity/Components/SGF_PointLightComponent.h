#pragma once
#include "SGF_Component.h"

class SGF_PointLightComponent : public SGF_Component
{
	COMPONENT_DECLARE(SGF_PointLightComponent, "PointLightComponent");
public:
	SGF_PointLightComponent();
	~SGF_PointLightComponent();
};

