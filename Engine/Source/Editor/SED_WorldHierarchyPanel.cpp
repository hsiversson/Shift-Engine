#include "SED_WorldHierarchyPanel.h"
#include "GameFramework/GameWorld/SGF_World.h"

SED_WorldHierarchyPanel::SED_WorldHierarchyPanel(const SC_Ref<SGF_World>& aWorld)
	: mWorld(aWorld)
	, mSelectedEntity(nullptr)
{

}

SED_WorldHierarchyPanel::~SED_WorldHierarchyPanel()
{

}

void SED_WorldHierarchyPanel::OnRender()
{
	ImGui::Begin("World Hierarchy");

	for (SC_Ref<SGF_Level>& level : mWorld->mLevels)
	{
		if (ImGui::TreeNodeEx("Unnamed Level", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth))
		{
			for (SC_Ref<SGF_Entity>& entity : level->mEntities)
			{
				DrawEntityNode(entity.get());
			}
			ImGui::TreePop();
		}
	}

	ImGui::End();
}

SGF_Entity* SED_WorldHierarchyPanel::GetSelected() const
{
	return mSelectedEntity;
}

void SED_WorldHierarchyPanel::DrawEntityNode(SGF_Entity* aEntity)
{
	ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (aEntity == mSelectedEntity)
		treeNodeFlags |= ImGuiTreeNodeFlags_Selected;

	bool opened = ImGui::TreeNodeEx(aEntity->GetName().c_str(), treeNodeFlags);

	if (ImGui::IsItemClicked())
		mSelectedEntity = aEntity;

	bool entityDeleted = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Delete"))
			entityDeleted = true;

		ImGui::EndPopup();
	}

	for (SGF_Entity* child : aEntity->mChildren)
	{
		DrawEntityNode(child);
	}

	if (opened)
	{
		ImGui::TreePop();
	}
}
