#include "SED_PropertiesPanel.h"

#include "SED_PropertyDrawDeclarations.h"

#include "GameFramework/GameWorld/SGF_World.h"
#include "GameFramework/Entity/SGF_Entity.h"
#include "GameFramework/Entity/Components/SGF_EntityIdComponent.h"

static void DrawComponent(const SGF_ComponentId& aComponentId, SGF_Entity* aEntity)
{
	if (aComponentId == SGF_EntityIdComponent::Id())
		return;

	if (SGF_Component* component = aEntity->GetComponent(aComponentId))
	{
		static constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		bool isOpen = ImGui::TreeNodeEx(&aComponentId, treeNodeFlags, component->GetName());
		ImGui::PopStyleVar();

		if (isOpen)
		{
			ImGui::PushID(component->GetName());
			ImGui::Columns(2);

			const SC_Array<SGF_PropertyBase*>& properties = component->GetProperties();
			for (SGF_PropertyBase* property : properties)
			{
				SED_DrawProperty(property->GetType(), property->GetData(), property->GetName());
			}

			ImGui::Columns(1);
			ImGui::PopID();
			ImGui::TreePop();
		}
	}
}

SED_PropertiesPanel::SED_PropertiesPanel()
	: mSelectedEntity(nullptr)
{

}

SED_PropertiesPanel::~SED_PropertiesPanel()
{

}

void SED_PropertiesPanel::OnRender()
{
	ImGui::Begin("Properties");

	if (mSelectedEntity != nullptr)
	{
		ImGui::BeginTable("##entityNameTable", 2); 
		ImGui::TableSetupColumn("##0", ImGuiTableColumnFlags_WidthFixed, 64.0f);
		ImGui::TableSetupColumn("##1", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableNextColumn();
		ImGui::Text("Name"); 
		ImGui::TableNextColumn();

		std::string name(mSelectedEntity->GetName());
		name.reserve(256);

		ImGui::PushItemWidth(ImGui::CalcItemWidth());
		if (ImGui::InputText("##entityName", name.data(), 256, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			mSelectedEntity->SetName(name);
		}
		ImGui::PopItemWidth();

		ImGui::EndTable();
		ImGui::Separator();

		const SGF_ComponentFactory::RegistryMap& componentRegistry = SGF_ComponentFactory::GetComponentRegistryMap();

		for (auto& pair : componentRegistry)
		{
			DrawComponent(pair.second, mSelectedEntity);
		}
	}

	ImGui::End();
}

void SED_PropertiesPanel::SetSelectedEntity(SGF_Entity* aEntity)
{
	mSelectedEntity = aEntity;
}

SGF_Entity* SED_PropertiesPanel::GetSelectedEntity() const
{
	return mSelectedEntity;
}
