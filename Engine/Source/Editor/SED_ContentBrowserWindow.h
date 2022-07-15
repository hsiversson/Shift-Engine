#pragma once
#include "SED_Window.h"
#include "SED_Thumbnail.h"

class SED_ContentBrowserWindow : public SED_Window
{
public:
	SED_ContentBrowserWindow();
	~SED_ContentBrowserWindow();

	const char* GetWindowName() const override { return "Content Browser"; }
	bool HasRightClickMenu() const override { return true; }
protected:
	void OnDraw() override;

private:
	void DrawFolderEntry(const char* aFolderName);
	void DrawAssetEntry(const char* aAssetName, const SED_AssetType& aType, void* aAssetPtr = nullptr, SC_SizeT aAssetDataSize = 0);

private:
	SED_Thumbnail mThumbnailGenerator;
};

