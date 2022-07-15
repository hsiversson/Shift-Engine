#include "SED_Icons.h"
#include "RenderCore/Interface/SR_RenderDevice.h"
#include "InputOutput/File/SC_EnginePaths.h"

SED_Icons* SED_Icons::gInstance = nullptr;

void SED_Icons::Create()
{
	if (!gInstance)
		gInstance = new SED_Icons;
}

void SED_Icons::Destroy()
{
	delete gInstance;
	gInstance = nullptr;
}

SED_Icons* SED_Icons::Get()
{
	return gInstance;
}

SED_Icons::SED_Icons()
{
	mIconsByType[static_cast<uint32>(IconType::Visible)] = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Editor_Icon_Visible.dds");
	mIconsByType[static_cast<uint32>(IconType::NonVisible)] = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Editor_Icon_NonVisible.dds");
}

SED_Icons::~SED_Icons()
{

}

SR_Texture* SED_Icons::GetIconByType(const IconType& aType) const
{
	return mIconsByType[static_cast<uint32>(aType)];
}

