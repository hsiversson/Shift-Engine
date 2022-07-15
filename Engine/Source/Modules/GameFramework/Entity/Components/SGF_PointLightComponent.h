#pragma once
#include "SGF_Component.h"
#include "Graphics/Lighting/SGfx_Light.h"

class SGF_PointLightComponent : public SGF_Component
{
	COMPONENT_DECLARE(SGF_PointLightComponent, "PointLight");
public:
	SGF_PointLightComponent();
	~SGF_PointLightComponent();

	void OnCreate() override;
	void OnUpdate() override;
	void OnDestroy() override;
//private:
	SC_Ref<SGfx_PointLight> mPointLight;

	SGF_PROPERTY(SC_Color, mColor, "Color");
	SGF_PROPERTY(float, mRange, "Range");
	SGF_PROPERTY(float, mIntensity, "Intensity");
	SGF_PROPERTY(float, mSourceRadius, "Source Radius");
	SGF_PROPERTY(float, mSourceRadiusSoft, "Soft Source Radius");
	SGF_PROPERTY(bool, mCastShadow, "Cast Shadow");
};

