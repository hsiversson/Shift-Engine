#include "SED_ContentBrowserWindow.h"
#include "Graphics/Material/SGfx_MaterialInstance.h"
#include "imgui_internal.h"

SED_ContentBrowserWindow::SED_ContentBrowserWindow()
{
	mThumbnailGenerator.Init();
}

SED_ContentBrowserWindow::~SED_ContentBrowserWindow()
{

}

void SED_ContentBrowserWindow::OnDraw()
{
	SC_PROFILER_FUNCTION();
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

	SR_Texture* thumbnailTexture = mThumbnailGenerator.GetDefaultThumbnail(SED_AssetType::Texture);
	DrawAssetEntry("TestTexture0", SED_AssetType::Texture, &thumbnailTexture, sizeof(thumbnailTexture));
	ImGui::NextColumn();
	DrawAssetEntry("TestMaterial0", SED_AssetType::Material);

	ImGui::Columns(1);
}

void SED_ContentBrowserWindow::DrawFolderEntry(const char* aFolderName)
{
	static const ImVec2 itemSize(192.f, 192.f);
	const ImVec2 currentPos = ImGui::GetCursorScreenPos();
	ImVec2 currentPos2;
	const ImGuiStyle& style = ImGui::GetStyle();
	const ImGuiContext& context = *ImGui::GetCurrentContext();
	const float labelHeight = context.FontSize;

	ImRect buttonRect = ImRect(currentPos.x, currentPos.y, currentPos.x + itemSize.x, currentPos.y + itemSize.y);
	ImRect labelRect = ImRect(buttonRect.Min.x, buttonRect.Max.y - labelHeight - style.FramePadding.y, buttonRect.Max.x, buttonRect.Max.y);

	SED_PushId(aFolderName);

	// Thumbnail
	{
		SED_ImageButton(mThumbnailGenerator.GetFolderThumbnail(), itemSize); 
		if (ImGui::BeginPopupContextItem())
		{
			SED_Selectable("Right Click Folder");

			ImGui::EndPopup();
		}
	}
	// Label
	{
		ImGui::TextWrapped(aFolderName);
	}

	SED_PopId();
}

void SED_ContentBrowserWindow::DrawAssetEntry(const char* aAssetName, const SED_AssetType& aType, void* aAssetPtr, SC_SizeT aAssetDataSize)
{
	static const ImVec2 itemSize(192.f, 192.f);
	const ImVec2 currentPos = ImGui::GetCursorScreenPos();
	ImVec2 currentPos2;
	const ImGuiStyle& style = ImGui::GetStyle();
	const ImGuiContext& context = *ImGui::GetCurrentContext();
	const float labelHeight = context.FontSize;

	ImRect buttonRect = ImRect(currentPos.x, currentPos.y, currentPos.x + itemSize.x, currentPos.y + itemSize.y);
	ImRect labelRect = ImRect(buttonRect.Min.x, buttonRect.Max.y - labelHeight - style.FramePadding.y, buttonRect.Max.x, buttonRect.Max.y);

	SED_PushId(aAssetName);

	// Thumbnail
	{
		SR_Texture* thumbnailTexture = mThumbnailGenerator.GetDefaultThumbnail(aType);
		ImGui::ImageButton(thumbnailTexture, itemSize);

		if (ImGui::BeginPopupContextItem())
		{
			SED_Selectable("Right Click Item");

			ImGui::EndPopup();
		}

		if (SED_BeginDragDropSource())
		{
			const char* dataTag = nullptr;
			switch (aType)
			{
			case SED_AssetType::Mesh:
				dataTag = "MeshInstanceDrag";
				break;
			case SED_AssetType::Model:
				dataTag = "ModelDrag";
				break;
			case SED_AssetType::Texture:
				dataTag = "TextureDrag";
				break;
			case SED_AssetType::Material:
				dataTag = "MaterialInstanceDrag";
				break;
			case SED_AssetType::Script:
				dataTag = "ScriptDrag";
				break;
			case SED_AssetType::Font:
				dataTag = "FontDrag";
				break;
			case SED_AssetType::DataTable:
				dataTag = "DataTableDrag";
				break;
			case SED_AssetType::Level:
				dataTag = "LevelDrag";
				break;
			case SED_AssetType::Project:
				dataTag = "ProjectDrag";
				break;
			}

			if (dataTag)
			{
				SED_PushDragDropPayload(dataTag, aAssetPtr, aAssetDataSize);
				SED_Image(thumbnailTexture, { 64.f, 64.f });
			}
			SED_EndDragDropSource();
		}
	}
	// Label
	{
		ImGui::TextWrapped(aAssetName);
	}

	SED_PopId();
}
