#pragma once
#include "SGF_Component.h"

class SGF_CameraComponent : public SGF_Component
{
	COMPONENT_DECLARE(SGF_CameraComponent, "CameraComponent");
public:
	SGF_CameraComponent();
	~SGF_CameraComponent();
};

