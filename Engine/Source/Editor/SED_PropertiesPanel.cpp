#include "SED_PropertiesPanel.h"

#include "GameFramework/GameWorld/SGF_World.h"
#include "GameFramework/Entity/SGF_Entity.h"
#include "GameFramework/Entity/Components/SGF_EntityIdComponent.h"
#include "Graphics/Material/SGfx_MaterialInstance.h"
#include "Graphics/Mesh/SGfx_MeshInstance.h"
#include "RenderCore/Interface/SR_Texture.h"
#include "imgui_internal.h"

SED_PropertiesPanel::SED_PropertiesPanel()
	: mSelectedEntity(nullptr)
{

}

SED_PropertiesPanel::SED_PropertiesPanel(const SC_Ref<SGF_World>& aWorld)
	: mSelectedEntity(nullptr)
	, mWorld(aWorld)
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

void SED_PropertiesPanel::DrawComponent(const SGF_ComponentId& aComponentId, SGF_Entity* aEntity) const
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
				DrawProperty(property->GetType(), property->GetData(), property->GetName(), property->GetResetData());
			}

			ImGui::Columns(1);
			ImGui::PopID();
			ImGui::TreePop();
		}
	}
}

void SED_PropertiesPanel::DrawProperty(const SGF_PropertyBase::Type& aType, void* aData, const char* aPropertyName, const void* aResetData) const
{
	ImGui::PushID(aPropertyName);
	ImGui::Text(aPropertyName);
	ImGui::NextColumn();

	switch (aType)
	{
	case SGF_PropertyBase::Type::Bool:
	{
		bool* boolPtr = static_cast<bool*>(aData);
		DrawPropertyInternal(aPropertyName, *boolPtr);
		break;
	}
	case SGF_PropertyBase::Type::Int:
	{
		int32* intPtr = static_cast<int32*>(aData);
		DrawPropertyInternal(aPropertyName, *intPtr);
		break;
	}
	case SGF_PropertyBase::Type::Uint:
	{
		uint32* uintPtr = static_cast<uint32*>(aData);
		DrawPropertyInternal(aPropertyName, *uintPtr);
		break;
	}
	case SGF_PropertyBase::Type::Float:
	{
		float* floatPtr = static_cast<float*>(aData);
		DrawPropertyInternal(aPropertyName, *floatPtr);
		break;
	}
	case SGF_PropertyBase::Type::Vector:
	{
		SC_Vector* vectorPtr = static_cast<SC_Vector*>(aData);
		const SC_Vector* resetValPtr = static_cast<const SC_Vector*>(aResetData);
		DrawPropertyInternal(aPropertyName, *vectorPtr, *resetValPtr);
		break;
	}
	case SGF_PropertyBase::Type::Quaternion:
	{
		SC_Quaternion* quatPtr = static_cast<SC_Quaternion*>(aData);
		const SC_Quaternion* resetValPtr = static_cast<const SC_Quaternion*>(aResetData);
		DrawPropertyInternal(aPropertyName, *quatPtr, *resetValPtr);
		break;
	}
	case SGF_PropertyBase::Type::Texture:
	{
		SC_Ref<SR_Texture>* texPtr = static_cast<SC_Ref<SR_Texture>*>(aData);
		DrawPropertyInternal(aPropertyName, *texPtr);
		break;
	}
	case SGF_PropertyBase::Type::Material:
	{
		SC_Ref<SGfx_MaterialInstance>* matPtr = static_cast<SC_Ref<SGfx_MaterialInstance>*>(aData);
		DrawPropertyInternal(aPropertyName, *matPtr);
		break;
	}
	case SGF_PropertyBase::Type::Mesh:
	{
		SC_Ref<SGfx_MeshInstance>* meshPtr = static_cast<SC_Ref<SGfx_MeshInstance>*>(aData);
		DrawPropertyInternal(aPropertyName, *meshPtr);
		break;
	}
	case SGF_PropertyBase::Type::EntityRef:
	{
		SC_Ref<SGF_Entity>* entityRefPtr = static_cast<SC_Ref<SGF_Entity>*>(aData);
		DrawPropertyInternal(aPropertyName, *entityRefPtr);
		break;
	}
	}

	ImGui::NextColumn();
	ImGui::PopID();
}

////////////////////////////////
// Native Types
void SED_PropertiesPanel::DrawPropertyInternal(const char* aName, bool& aProperty) const
{
	std::string label("##_checkbox_");
	label += aName;
	ImGui::Checkbox(label.c_str(), &aProperty);
}

void SED_PropertiesPanel::DrawPropertyInternal(const char* aName, int32& aProperty) const
{
	std::string label("##_checkbox_");
	label += aName;
	ImGui::DragInt(label.c_str(), &aProperty);
}

void SED_PropertiesPanel::DrawPropertyInternal(const char* aName, uint32& aProperty) const
{
	std::string label("##_checkbox_");
	label += aName;
	ImGui::DragScalar(label.c_str(), ImGuiDataType_U32, &aProperty);
}

void SED_PropertiesPanel::DrawPropertyInternal(const char* aName, float& aProperty) const
{
	std::string label("##_checkbox_");
	label += aName;
	ImGui::DragFloat(label.c_str(), &aProperty);
}

////////////////////////////////
//	Vector
static void DrawPropertyVectorComponent(const char* aName, const ImVec2& aButtonSize, float& aOutComponent, const ImVec4& aColor, const float aResetValue)
{
	const ImVec4 hoverColor = { aColor.x + 0.1f, aColor.y + 0.1f, aColor.z + 0.1f, 1.0f };

	ImGui::PushStyleColor(ImGuiCol_Button, aColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, aColor);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 8 });
	if (ImGui::Button(aName, aButtonSize))
		aOutComponent = aResetValue;
	ImGui::PopStyleColor(3);

	ImGui::SameLine();

	std::string label("##_VectorComponent_");
	label += aName;
	ImGui::DragFloat(label.c_str(), &aOutComponent, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopStyleVar();
}

void SED_PropertiesPanel::DrawPropertyInternal(const char* /*aName*/, SC_Vector& aProperty, const SC_Vector& aResetValue) const
{
	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 4, 6 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 5.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	const ImVec4 xColor = ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f };
	DrawPropertyVectorComponent("X", buttonSize, aProperty.x, xColor, aResetValue.x);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	const ImVec4 yColor = ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f };
	DrawPropertyVectorComponent("Y", buttonSize, aProperty.y, yColor, aResetValue.y);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	const ImVec4 zColor = ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f };
	DrawPropertyVectorComponent("Z", buttonSize, aProperty.z, zColor, aResetValue.z);
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();
}

////////////////////////////////
//	Quaternion / Rotation
void SED_PropertiesPanel::DrawPropertyInternal(const char* aName, SC_Quaternion& aProperty, const SC_Quaternion& aResetValue) const
{
	SC_Vector anglesProperty = aProperty.AsEulerAngles();
	DrawPropertyInternal(aName, anglesProperty, aResetValue.AsEulerAngles());
	aProperty = aProperty.FromEulerAngles(anglesProperty);
}

////////////////////////////////
//	Asset
void SED_PropertiesPanel::DrawPropertyInternal(const char* aName, SC_Ref<SR_Texture>& aProperty) const
{
	std::string label("##_texture_");
	label += aName;
	ImGui::ImageButton(aProperty.get(), { 128.f, 128.f });

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TextureDrag", ImGuiDragDropFlags_None))
		{
			SR_Texture* tex;
			SC_Memcpy(&tex, payload->Data, payload->DataSize);
			aProperty.reset(tex);
		}

		ImGui::EndDragDropTarget();
	}
}

void SED_PropertiesPanel::DrawPropertyInternal(const char* aName, SC_Ref<SGfx_MaterialInstance>& aProperty) const
{
	std::string label("##_material_");
	label += aName;
	ImGui::ImageButton(nullptr, { 128.f, 128.f }); 
	
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MaterialInstanceDrag", ImGuiDragDropFlags_None))
		{
			SGfx_MaterialInstance* mat;
			SC_Memcpy(&mat, payload->Data, payload->DataSize);
			aProperty.reset(mat);
		}

		ImGui::EndDragDropTarget();
	}
}

void SED_PropertiesPanel::DrawPropertyInternal(const char* aName, SC_Ref<SGfx_MeshInstance>& aProperty) const
{
	std::string label("##_mesh_");
	label += aName;
	ImGui::ImageButton(nullptr, { 128.f, 128.f });
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MeshInstanceDrag", ImGuiDragDropFlags_None))
		{
			SGfx_MeshInstance* mesh;
			SC_Memcpy(&mesh, payload->Data, payload->DataSize);
			aProperty.reset(mesh);
		}

		ImGui::EndDragDropTarget();
	}
}

void SED_PropertiesPanel::DrawPropertyInternal(const char* aName, SC_Ref<SGF_Entity>& aProperty) const
{
	std::string label("##_entity_");
	label += aName;

	if (mWorld)
	{
		if (aProperty)
			ImGui::Text("%s", aProperty->GetName().c_str());
		else
			ImGui::Text("<None>");

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityDrag", ImGuiDragDropFlags_None))
			{
				SGF_Entity* entity;
				SC_Memcpy(&entity, payload->Data, payload->DataSize);
				aProperty.reset(entity);
			}

			ImGui::EndDragDropTarget();
		}
	}
	else
		ImGui::Text("<None>");
}