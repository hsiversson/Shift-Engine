#include "SED_PropertiesPanel.h"

#include "SED_PropertyDrawDeclarations.h"

#include "GameFramework/GameWorld/SGF_World.h"
#include "GameFramework/Entity/SGF_Entity.h"
#include "GameFramework/Entity/Components/SGF_TransformComponent.h"

template<typename ComponentType, typename DrawFuncSig>
static void DrawComponent(const char* aComponentName, SGF_Entity* aEntity, DrawFuncSig aDrawFunc)
{
	if (aEntity->HasComponent<ComponentType>())
	{
		static constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		ComponentType& component = *(aEntity->GetComponent<ComponentType>());

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		bool isOpen = ImGui::TreeNodeEx((void*)typeid(ComponentType).hash_code(), treeNodeFlags, aComponentName);
		ImGui::PopStyleVar();

		if (isOpen)
		{
			aDrawFunc(component);
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
		ImGui::Text("Name: %s", mSelectedEntity->GetName().c_str());
		ImGui::Separator();

		//const SGF_ComponentFactory::RegistryMap& componentRegistry = SGF_ComponentFactory::GetComponentRegistryMap();

		//for (auto& pair : componentRegistry)
		//{
		//	DrawComponent(pair.second, pair.first.c_str(), ComponentDrawFunc);
		//}


		DrawComponent<SGF_TransformComponent>("Transform", mSelectedEntity, [](SGF_TransformComponent& aComponent)
			{
				DrawProperty("Position", aComponent.mPosition);
				DrawProperty("Rotation", aComponent.mRotation);
				DrawProperty("Scale", aComponent.mScale);
			});
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
