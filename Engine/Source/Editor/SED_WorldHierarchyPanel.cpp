#include "SED_WorldHierarchyPanel.h"
#include "GameFramework/GameWorld/SGF_World.h"
#include "GameFramework/Entity/Components/SGF_EntityIdComponent.h"

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

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
		mSelectedEntity = nullptr;

	for (SC_Ref<SGF_Level>& level : mWorld->mLevels)
	{
		if (ImGui::TreeNodeEx("Unnamed Level", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth))
		{
			for (SC_Ref<SGF_Entity>& entity : level->mEntities)
			{
				if (!entity->GetParent())
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
	if (aEntity->GetChildren().IsEmpty())
		treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;

	bool opened = ImGui::TreeNodeEx(aEntity->GetName().c_str(), treeNodeFlags); 
	
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("EntityDrag", &aEntity, sizeof(aEntity));
		ImGui::Text("%s", aEntity->GetName().c_str());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityDrag", ImGuiDragDropFlags_None))
		{
			SGF_Entity* entity;
			SC_Memcpy(&entity, payload->Data, payload->DataSize);

			if (entity != aEntity)
			{
				aEntity->AddChild(entity);
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsItemClicked())
		mSelectedEntity = aEntity;

	bool entityDeleted = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Delete"))
			entityDeleted = true;

		ImGui::EndPopup();
	}

	if (opened)
	{
		for (SGF_Entity* child : aEntity->mChildren)
		{
			DrawEntityNode(child);
		}

		ImGui::TreePop();
	}
}
