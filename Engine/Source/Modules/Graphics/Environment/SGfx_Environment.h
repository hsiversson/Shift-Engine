#pragma once
#include "Graphics/Misc/SGfx_Surface.h"
#include "Graphics/Environment/SGfx_Sky.h"

class SR_Texture;
class SR_BufferResource;
class SR_ShaderState;
class SR_CommandList;
class SGfx_Camera;

struct alignas(16) SGfx_EnvironmentConstants
{
	SGfx_EnvironmentConstants() : mSunLightIntensity(1.0f) {}

	SC_Vector mSunLightDirection;
	float mSunLightIntensity;

	SC_LinearColor mSunLightColor;

	SGfx_SkyAtmosphereConstants mSkyAtmosphereConstants;
	SGfx_SkyIrradianceConstants mySkyIrradianceConstants;
};

/**
* 
*	SGfx_Environment owns multiple environmental subsystems
*	such as Wind, Sky and Sun...
* 
*/

class SGfx_Environment
{
public:
	SGfx_Environment();
	~SGfx_Environment();

	const SGfx_EnvironmentConstants& GetConstants() const;

	SC_Vector& GetSunDirection();
	const SC_Vector& GetSunDirection() const;
	SC_LinearColor& GetSunColor();
	const SC_LinearColor& GetSunColor() const;
	float& GetSunIntensity();
	const float& GetSunIntensity() const;

	void UpdateConstants(const SGfx_Camera& aWorldCamera);

	SGfx_Sky* GetSky() const { return mSky.get(); }

	void ComputeSkyAtmosphereLUTs(SR_CommandList* aCmdList);

private:
	SGfx_EnvironmentConstants mConstants;
	SC_UniquePtr<SGfx_Sky> mSky;
};

