#include "SGfx_DefaultTextures.h"

SGfx_DefaultTextures* SGfx_DefaultTextures::gInstance = nullptr;

SGfx_DefaultTextures::SGfx_DefaultTextures()
{

}

SGfx_DefaultTextures::~SGfx_DefaultTextures()
{

}

bool SGfx_DefaultTextures::Init()
{
	if (!gInstance) 
	{
		gInstance = new SGfx_DefaultTextures();
		gInstance->mWhite1x1 = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_White_1x1.dds");
		gInstance->mBlack1x1 = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Black_1x1.dds");
		gInstance->mGrey1x1 = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Grey_1x1.dds");
		gInstance->mFlatNormalMap1x1 = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Normal_1x1.dds");
	}
	return true;
}

void SGfx_DefaultTextures::Destroy()
{
	delete gInstance;
	gInstance = nullptr;
}

const SC_Ref<SR_Texture>& SGfx_DefaultTextures::GetWhite1x1()
{
	SC_ASSERT(gInstance, "Instance not created!");
	return gInstance->mWhite1x1;
}

const SC_Ref<SR_Texture>& SGfx_DefaultTextures::GetBlack1x1()
{
	SC_ASSERT(gInstance, "Instance not created!");
	return gInstance->mBlack1x1;
}

const SC_Ref<SR_Texture>& SGfx_DefaultTextures::GetGrey1x1()
{
	SC_ASSERT(gInstance, "Instance not created!");
	return gInstance->mGrey1x1;
}

const SC_Ref<SR_Texture>& SGfx_DefaultTextures::GetFlatNormalMap1x1()
{
	SC_ASSERT(gInstance, "Instance not created!");
	return gInstance->mFlatNormalMap1x1;
}
