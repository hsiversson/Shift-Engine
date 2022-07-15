#include "SED_WorldHierarchyWindow.h"

#include "SED_Icons.h"

#include "GameFramework/GameWorld/SGF_World.h"
#include "GameFramework/Entity/Components/SGF_EntityIdComponent.h"

SED_WorldHierarchyWindow::SED_WorldHierarchyWindow(const SC_Ref<SGF_World>& aWorld)
	: mWorld(aWorld)
{

}

SED_WorldHierarchyWindow::~SED_WorldHierarchyWindow()
{

}

const SGF_Entity& SED_WorldHierarchyWindow::GetSelected() const
{
	return mSelectedEntity;
}

void SED_WorldHierarchyWindow::OnDraw()
{
	if (ImGui::BeginPopupContextWindow())
	{
		ImGui::Button("New Entity");
		ImGui::EndPopup();
	}

	ImGui::BeginTable("##propertyTable", 3, ImGuiTableFlags_Resizable);
	ImGui::TableSetupColumn("##visibilityColumn", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_IndentDisable, 24.0f);
	ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_IndentEnable, 128.0f);
	ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_IndentDisable);
	ImGui::TableHeadersRow();

	SGF_EntityManager* entityManager = mWorld->GetEntityManager();

	SC_Array<SGF_Entity> entities;
	entityManager->GetEntities(entities);

	for (SGF_Entity& entity : entities)
	{
		DrawEntityNode(entity);
	}

	//for (SC_Ref<SGF_Level>& level : mWorld->mLevels)
	//{
	//	ImGui::TableNextRow();
	//
	//	ImGui::TableSetColumnIndex(0);
	//	if (level->IsVisible())
	//	{
	//		if (ImGui::ImageButton(SED_Icons::Get()->GetIconByType(SED_Icons::IconType::Visible), { 20.f, 20.f }))
	//			level->SetVisible(false);
	//	}
	//	else
	//	{
	//		if (ImGui::ImageButton(SED_Icons::Get()->GetIconByType(SED_Icons::IconType::NonVisible), { 20.f, 20.f }))
	//			level->SetVisible(true);
	//	}
	//	ImGui::TableSetColumnIndex(1);
	//
	//	bool open = ImGui::TreeNodeEx("Unnamed Level", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth);
	//	if (ImGui::BeginPopupContextItem())
	//	{
	//		ImGui::Button("Right Clicked Level");
	//		ImGui::EndPopup();
	//	}
	//
	//	ImGui::TableSetColumnIndex(2);
	//	ImGui::TextDisabled("Level");
	//
	//	if (open)
	//	{
	//		for (SGF_Entity& entity : level->mEntities)
	//		{
	//			//if (!entity->GetParent())
	//				DrawEntityNode(entity);
	//		}
	//		ImGui::TreePop();
	//	}
	//}
	ImGui::EndTable();
}

void SED_WorldHierarchyWindow::DrawEntityNode(const SGF_Entity& aEntity)
{
	ImGui::TableNextRow();

	SGF_EntityIdComponent* id = aEntity.GetComponent<SGF_EntityIdComponent>();
	std::string uuid;
	id->GetUUID().AsString(uuid);
	ImGui::PushID(uuid.c_str());

	ImGui::TableSetColumnIndex(0);

	//if (aEntity->IsVisible())
	//{
	//	if (ImGui::ImageButton(SED_Icons::Get()->GetIconByType(SED_Icons::IconType::Visible), { 20.f, 20.f }))
	//		aEntity->SetVisible(false);
	//}
	//else
	{
		ImGui::ImageButton(SED_Icons::Get()->GetIconByType(SED_Icons::IconType::NonVisible), { 20.f, 20.f });
			//if (ImGui::ImageButton(SED_Icons::Get()->GetIconByType(SED_Icons::IconType::NonVisible), { 20.f, 20.f }))
			//	aEntity.SetVisible(true);
	}

	ImGui::TableSetColumnIndex(1);

	ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
	if (aEntity == mSelectedEntity)
		treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
	//if (aEntity->GetChildren().IsEmpty())
	treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;

	bool opened = ImGui::TreeNodeEx(aEntity.GetComponent<SGF_EntityNameComponent>()->mName.c_str(), treeNodeFlags); 
	
	if (ImGui::BeginDragDropSource())
	{
		SGF_EntityHandle handle;
		ImGui::SetDragDropPayload("EntityDrag", &handle, sizeof(handle));
		const char* name = "NAME";
		ImGui::Text("%s", name);
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityDrag", ImGuiDragDropFlags_None))
		{
			SGF_EntityHandle entity;
			SC_Memcpy(&entity, payload->Data, payload->DataSize);

			if (entity != SGF_InvalidEntityHandle)
			{
				//aEntity->AddChild(entity);
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsItemClicked())
		mSelectedEntity = aEntity;

	bool entityDeleted = false;
	if (ImGui::BeginPopupContextItem())
	{
		mSelectedEntity = aEntity;

		if (ImGui::MenuItem("Delete"))
			entityDeleted = true;

		ImGui::EndPopup();
	}

	ImGui::TableSetColumnIndex(2);
	ImGui::TextDisabled("Entity");

	if (opened)
	{
		//for (SGF_Entity* child : aEntity->mChildren)
		//	DrawEntityNode(child);

		ImGui::TreePop();
	}

	ImGui::PopID();
}
