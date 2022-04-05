#include "SGfx_Environment.h"
#include "Graphics/View/SGfx_Camera.h"
#include "Graphics/Misc/SGfx_Shapes.h"
#include "Graphics/Misc/SGfx_DefaultTextures.h"
#include "RenderCore/Interface/SR_CommandList.h"

SGfx_Environment::SGfx_Environment()
{
	mConstants.mSunLightDirection = SC_Vector(0.45f, 0.80f, 0.05f);
	mConstants.mSunLightIntensity = 30.0f;

	mConstants.mSunLightColor = SC_Vector(0.99f, 0.84f, 0.55f);

	mSky = SC_MakeUnique<SGfx_Sky>();
	if (!mSky->Init())
		mSky = nullptr;
}

SGfx_Environment::~SGfx_Environment()
{

}

const SGfx_EnvironmentConstants& SGfx_Environment::GetConstants() const
{
	return mConstants;
}

SC_Vector& SGfx_Environment::GetSunDirection()
{
	return mConstants.mSunLightDirection;
}

const SC_Vector& SGfx_Environment::GetSunDirection() const
{
	return mConstants.mSunLightDirection;
}

SC_Vector& SGfx_Environment::GetSunColor()
{
	return mConstants.mSunLightColor;
}

const SC_Vector& SGfx_Environment::GetSunColor() const
{
	return mConstants.mSunLightColor;
}

float& SGfx_Environment::GetSunIntensity()
{
	return mConstants.mSunLightIntensity;
}

const float& SGfx_Environment::GetSunIntensity() const
{
	return mConstants.mSunLightIntensity;
}

void SGfx_Environment::UpdateConstants(const SGfx_Camera& aWorldCamera)
{
	mSky->UpdateConstants(aWorldCamera);

	mConstants.mSkyAtmosphereConstants = mSky->GetSkyAtmosphereConstants();
	mConstants.mySkyIrradianceConstants = mSky->GetSkyIrradianceConstants();
	mConstants.mSunLightDirection.Normalize();
}

void SGfx_Environment::ComputeSkyAtmosphereLUTs(SR_CommandList* aCmdList)
{
	if (mSky)
		mSky->ComputeSkyAtmosphereLUTs(aCmdList);
}
