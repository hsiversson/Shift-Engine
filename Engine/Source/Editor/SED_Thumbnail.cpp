#include "SED_Thumbnail.h"
#include "RenderCore/Interface/SR_RenderDevice.h"
#include "InputOutput/File/SC_EnginePaths.h"

SED_Thumbnail::SED_Thumbnail()
{

}

SED_Thumbnail::~SED_Thumbnail()
{

}

bool SED_Thumbnail::Init()
{

	mDefaultThumbnail[static_cast<uint32>(SED_AssetType::Unknown)] = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Editor_Asset_Empty.dds");
	mDefaultThumbnail[static_cast<uint32>(SED_AssetType::Texture)] = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Editor_Asset_Texture.dds");
	mDefaultThumbnail[static_cast<uint32>(SED_AssetType::Material)] = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Editor_Asset_Material.dds");
	mDefaultThumbnail[static_cast<uint32>(SED_AssetType::Mesh)] = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Editor_Asset_Mesh.dds");

	mFolderThumbnail = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Editor_Asset_Folder.dds");

	return true;
}

SC_Ref<SR_Texture> SED_Thumbnail::GetDefaultThumbnail(const SED_AssetType& aType) const
{
	return mDefaultThumbnail[static_cast<uint32>(aType)];
}

SC_Ref<SR_Texture> SED_Thumbnail::GetFolderThumbnail() const
{
	return mFolderThumbnail;
}
