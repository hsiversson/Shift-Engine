#pragma once
#include "SED_Asset.h"

class SR_Texture;

class SED_Thumbnail
{
public:
	SED_Thumbnail();
	~SED_Thumbnail();

	bool Init();

	SC_Ref<SR_Texture> GetDefaultThumbnail(const SED_AssetType& aType) const;
	SC_Ref<SR_Texture> GetFolderThumbnail() const;
private:
	SC_Ref<SR_Texture> mDefaultThumbnail[static_cast<uint32>(SED_AssetType::COUNT)];
	SC_Ref<SR_Texture> mFolderThumbnail;
};

