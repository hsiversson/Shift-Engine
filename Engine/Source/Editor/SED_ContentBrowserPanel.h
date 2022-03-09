#pragma once
#include "SED_Panel.h"
#include "SED_Thumbnail.h"

class SED_ContentBrowserPanel : public SED_Panel
{
public:
	SED_ContentBrowserPanel();
	~SED_ContentBrowserPanel();

	void OnRender() override;

private:
	void DrawFolderEntry(const char* aFolderName);
	void DrawAssetEntry(const char* aAssetName, const SED_AssetType& aType, void* aAssetPtr = nullptr, SC_SizeT aAssetDataSize = 0);
private:
	SED_Thumbnail mThumbnailGenerator;
};

