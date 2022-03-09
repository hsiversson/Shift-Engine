#include "SED_ContentBrowserPanel.h"
#include "Graphics/Material/SGfx_MaterialInstance.h"
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

	SR_Texture* thumbnailTexture = mThumbnailGenerator.GetDefaultThumbnail(SED_AssetType::Texture).get();
	DrawAssetEntry("TestTexture0", SED_AssetType::Texture, &thumbnailTexture, sizeof(thumbnailTexture));
	ImGui::NextColumn();
	DrawAssetEntry("TestMaterial0", SED_AssetType::Material);

	ImGui::Columns(1);
	ImGui::End();
}

void SED_ContentBrowserPanel::DrawFolderEntry(const char* aFolderName)
{
	static const ImVec2 itemSize(192.f, 192.f);
	const ImVec2 currentPos = ImGui::GetCursorScreenPos();
	ImVec2 currentPos2;
	const ImGuiStyle& style = ImGui::GetStyle();
	const ImGuiContext& context = *ImGui::GetCurrentContext();
	const float labelHeight = context.FontSize;

	ImRect buttonRect = ImRect(currentPos.x, currentPos.y, currentPos.x + itemSize.x, currentPos.y + itemSize.y);
	ImRect labelRect = ImRect(buttonRect.Min.x, buttonRect.Max.y - labelHeight - style.FramePadding.y, buttonRect.Max.x, buttonRect.Max.y);

	ImGui::PushID(aFolderName);

	// Thumbnail
	{
		ImGui::ImageButton(mThumbnailGenerator.GetFolderThumbnail().get(), itemSize);
	}
	// Label
	{
		ImGui::TextWrapped(aFolderName);
	}

	ImGui::PopID();
}

void SED_ContentBrowserPanel::DrawAssetEntry(const char* aAssetName, const SED_AssetType& aType, void* aAssetPtr, SC_SizeT aAssetDataSize)
{
	static const ImVec2 itemSize(192.f, 192.f);
	const ImVec2 currentPos = ImGui::GetCursorScreenPos();
	ImVec2 currentPos2;
	const ImGuiStyle& style = ImGui::GetStyle();
	const ImGuiContext& context = *ImGui::GetCurrentContext();
	const float labelHeight = context.FontSize;

	ImRect buttonRect = ImRect(currentPos.x, currentPos.y, currentPos.x + itemSize.x, currentPos.y + itemSize.y);
	ImRect labelRect = ImRect(buttonRect.Min.x, buttonRect.Max.y - labelHeight - style.FramePadding.y, buttonRect.Max.x, buttonRect.Max.y);

	ImGui::PushID(aAssetName);

	// Thumbnail
	{
		SR_Texture* thumbnailTexture = mThumbnailGenerator.GetDefaultThumbnail(aType).get();
		ImGui::ImageButton(thumbnailTexture, itemSize);
		if (ImGui::BeginDragDropSource())
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
				ImGui::SetDragDropPayload(dataTag, aAssetPtr, aAssetDataSize);
				ImGui::Image(thumbnailTexture, { 64.f, 64.f });
			}
			ImGui::EndDragDropSource();
		}
	}
	// Label
	{
		ImGui::TextWrapped(aAssetName);
	}

	ImGui::PopID();
}
