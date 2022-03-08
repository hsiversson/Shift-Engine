#include "SED_ContentBrowserPanel.h"
#include "imgui_internal.h"

SED_ContentBrowserPanel::SED_ContentBrowserPanel()
{
	mThumbnailGenerator.Init();
}

SED_ContentBrowserPanel::~SED_ContentBrowserPanel()
{

}

void SED_ContentBrowserPanel::OnRender()
{
	ImGui::Begin("Content Browser");

	static float padding = 16.0f;
	static float thumbnailSize = 192.0f;
	float cellSize = thumbnailSize + padding;
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int32 columnCount = static_cast<int32>(SC_Max(panelWidth / cellSize, 1.0f));

	ImGui::Columns(columnCount, 0, false);

	for (uint32 i = 0; i < 10; ++i)
	{
		std::string n("TestFolder");
		n += std::to_string(i);
		DrawFolderEntry(n.c_str());
		ImGui::NextColumn();
	}
	DrawAssetEntry("TestTexture0", SED_AssetType::Texture);
	ImGui::NextColumn();
	DrawAssetEntry("TestMaterial0", SED_AssetType::Material);

	ImGui::Columns(1);
	ImGui::End();
}

void SED_ContentBrowserPanel::DrawFolderEntry(const char* aAssetName)
{
	DrawEntry(aAssetName, mThumbnailGenerator.GetFolderThumbnail().get());
}

void SED_ContentBrowserPanel::DrawAssetEntry(const char* aAssetName, const SED_AssetType& aType)
{
	DrawEntry(aAssetName, mThumbnailGenerator.GetDefaultThumbnail(aType).get());
}

void SED_ContentBrowserPanel::DrawEntry(const char* aEntryName, SR_Texture* aThumbnail)
{
	static const ImVec2 itemSize(192.f, 192.f);
	const ImVec2 currentPos = ImGui::GetCursorScreenPos();
	ImVec2 currentPos2;
	const ImGuiStyle& style = ImGui::GetStyle();
	const ImGuiContext& context = *ImGui::GetCurrentContext();
	const float labelHeight = context.FontSize;

	ImRect buttonRect = ImRect(currentPos.x, currentPos.y, currentPos.x + itemSize.x, currentPos.y + itemSize.y);
	ImRect labelRect = ImRect(buttonRect.Min.x, buttonRect.Max.y - labelHeight - style.FramePadding.y, buttonRect.Max.x, buttonRect.Max.y);

	ImGui::PushID(aEntryName);

	// Thumbnail
	{
		ImGui::ImageButton(aThumbnail, itemSize);
	}
	// Label
	{
		ImGui::TextWrapped(aEntryName);
	}

	ImGui::PopID();
}
