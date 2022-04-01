#pragma once
#include "SGF_Component.h"
#include "Graphics/Lighting/SGfx_Light.h"

class SGF_SpotLightComponent : public SGF_Component 
{
	COMPONENT_DECLARE(SGF_SpotLightComponent, "SpotLight");

public:
	SGF_SpotLightComponent();
	~SGF_SpotLightComponent();

	void OnCreate() override;
	void OnUpdate() override;
	void OnDestroy() override;

	//PROPERTY(float, mIntensity, SGF_PropertyAccess::Edit);

//private:
	SC_Ref<SGfx_SpotLight> mSpotLight;

	SGF_PROPERTY(SC_Color, mColor, "Color");
	SGF_PROPERTY(float, mRange, "Range");
	SGF_PROPERTY(float, mIntensity, "Intensity");
	SGF_PROPERTY(float, mSourceRadius, "Source Radius");
	SGF_PROPERTY(float, mSourceRadiusSoft, "Soft Source Radius");
	SGF_PROPERTY(float, mInnerAngle, "Inner Angle");
	SGF_PROPERTY(float, mOuterAngle, "Outer Angle");
};