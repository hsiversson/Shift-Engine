#include "SED_PropertiesPanel.h"

#include "GameFramework/GameWorld/SGF_World.h"
#include "GameFramework/Entity/SGF_Entity.h"
#include "GameFramework/Entity/Components/SGF_EntityIdComponent.h"
#include "GameFramework/Entity/Components/SGF_TransformComponent.h"
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

		if (ImGui::BeginCombo("##addComp", "Add Component"))
		{
			for (auto& pair : componentRegistry)
			{
				if (!mSelectedEntity->HasComponent(pair.second))
				{
					if (ImGui::Button(pair.first.c_str()))
						mSelectedEntity->AddComponent(pair.second);
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Separator();

		DrawComponent(SGF_TransformComponent::Id(), mSelectedEntity);

		for (auto& pair : componentRegistry)
		{
			if (pair.second != SGF_TransformComponent::Id())
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

			const SC_Array<SGF_PropertyHelperBase*>& properties2 = component->GetProperties();
			for (SGF_PropertyHelperBase* property : properties2)
			{
				DrawProperty(*property);
			}

			ImGui::Columns(1);
			ImGui::PopID();
			ImGui::TreePop();
		}
	}
}

void SED_PropertiesPanel::DrawProperty(SGF_PropertyHelperBase& aProperty) const
{
	ImGui::PushID(aProperty.GetName());
	ImGui::Text(aProperty.GetName());
	ImGui::NextColumn();

	switch (aProperty.GetType())
	{
	case SGF_PropertyHelperBase::Type::Bool:		DrawPropertyInternal(static_cast<SGF_PropertyHelper<bool>&>(aProperty)); break;
	case SGF_PropertyHelperBase::Type::Int:			DrawPropertyInternal(static_cast<SGF_PropertyHelper<int32>&>(aProperty)); break;
	case SGF_PropertyHelperBase::Type::Uint:		DrawPropertyInternal(static_cast<SGF_PropertyHelper<uint32>&>(aProperty)); break;
	case SGF_PropertyHelperBase::Type::Float:		DrawPropertyInternal(static_cast<SGF_PropertyHelper<float>&>(aProperty)); break;
	case SGF_PropertyHelperBase::Type::Vector:		DrawPropertyInternal(static_cast<SGF_PropertyHelper<SC_Vector>&>(aProperty)); break;
	case SGF_PropertyHelperBase::Type::Color:		DrawPropertyInternal(static_cast<SGF_PropertyHelper<SC_Color>&>(aProperty)); break;
	case SGF_PropertyHelperBase::Type::Quaternion:	DrawPropertyInternal(static_cast<SGF_PropertyHelper<SC_Quaternion>&>(aProperty)); break;
	case SGF_PropertyHelperBase::Type::Texture:		DrawPropertyInternal(static_cast<SGF_PropertyHelper<SC_Ref<SR_Texture>>&>(aProperty)); break;
	case SGF_PropertyHelperBase::Type::Material:	DrawPropertyInternal(static_cast<SGF_PropertyHelper<SC_Ref<SGfx_MaterialInstance>>&>(aProperty)); break;
	case SGF_PropertyHelperBase::Type::Mesh:		DrawPropertyInternal(static_cast<SGF_PropertyHelper<SC_Ref<SGfx_MeshInstance>>&>(aProperty)); break;
	case SGF_PropertyHelperBase::Type::EntityRef:	DrawPropertyInternal(static_cast<SGF_PropertyHelper<SC_Ref<SGF_Entity>>&>(aProperty)); break;
	}

	ImGui::NextColumn();
	ImGui::PopID();
}

////////////////////////////////
// Native Types
void SED_PropertiesPanel::DrawPropertyInternal(SGF_PropertyHelper<bool>& aProperty) const
{
	std::string label("##_bool_");
	label += aProperty.GetName();
	ImGui::Checkbox(label.c_str(), &aProperty.Get());
}

void SED_PropertiesPanel::DrawPropertyInternal(SGF_PropertyHelper<int32>& aProperty) const
{
	std::string label("##_int_");
	label += aProperty.GetName();
	ImGui::DragInt(label.c_str(), &aProperty.Get());
}

void SED_PropertiesPanel::DrawPropertyInternal(SGF_PropertyHelper<uint32>& aProperty) const
{
	std::string label("##_uint_");
	label += aProperty.GetName();
	ImGui::DragScalar(label.c_str(), ImGuiDataType_U32, &aProperty.Get());
}

void SED_PropertiesPanel::DrawPropertyInternal(SGF_PropertyHelper<float>& aProperty) const
{
	std::string label("##_float_");
	label += aProperty.GetName();
	ImGui::DragFloat(label.c_str(), &aProperty.Get());
}

////////////////////////////////
//	Vector
static void DrawPropertyVectorComponent(const char* aName, const ImVec2& aButtonSize, float& aOutComponent, const ImVec4& aColor, const float aMin, const float aMax, const float aSpeed, const float aResetValue)
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
	ImGui::DragFloat(label.c_str(), &aOutComponent, aSpeed, aMin, aMax, "%.3f");
	ImGui::PopStyleVar();
}

void SED_PropertiesPanel::DrawPropertyInternal(SGF_PropertyHelper<SC_Vector>& aProperty) const
{
	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 4, 6 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 5.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	SC_Vector& property = aProperty.Get();

	float resetValue = 0.0f;
	std::string resetValueStr = aProperty.GetMetaDataFromKey("resetvalue");
	if (!resetValueStr.empty())
		resetValue = std::stof(resetValueStr);

	float minValue = 0.0f;
	std::string minValueStr = aProperty.GetMetaDataFromKey("min");
	if (!minValueStr.empty())
		minValue = std::stof(minValueStr);

	float maxValue = 0.0f;
	std::string maxValueStr = aProperty.GetMetaDataFromKey("max");
	if (!maxValueStr.empty())
		maxValue = std::stof(maxValueStr);

	float speedValue = 0.01f;
	std::string speedValueStr = aProperty.GetMetaDataFromKey("floatdragspeed");
	if (!speedValueStr.empty())
		speedValue = std::stof(speedValueStr);

	const ImVec4 xColor = ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f };
	DrawPropertyVectorComponent("X", buttonSize, property.x, xColor, minValue, maxValue, speedValue, resetValue);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	const ImVec4 yColor = ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f };
	DrawPropertyVectorComponent("Y", buttonSize, property.y, yColor, minValue, maxValue, speedValue, resetValue);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	const ImVec4 zColor = ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f };
	DrawPropertyVectorComponent("Z", buttonSize, property.z, zColor, minValue, maxValue, speedValue, resetValue);
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();
}

void SED_PropertiesPanel::DrawPropertyInternal(SGF_PropertyHelper<SC_Color>& aProperty) const
{
	SC_Color& col = aProperty.Get();
	SC_LinearColor linear = SC_ConvertColorToLinear(col);
	if (ImGui::ColorEdit4("##_coloredit_", &linear.r, ImGuiColorEditFlags_Uint8))
	{
		col = SC_ConvertLinearToColor(linear);
	}
}

////////////////////////////////
//	Quaternion / Rotation
void SED_PropertiesPanel::DrawPropertyInternal(SGF_PropertyHelper<SC_Quaternion>& aProperty) const
{
	SC_Quaternion& quat = aProperty.Get();
	SC_Vector angles = quat.AsEulerAngles();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 4, 6 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 5.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	float resetValue = 0.0f;
	std::string resetValueStr = aProperty.GetMetaDataFromKey("resetvalue");
	if (!resetValueStr.empty())
		resetValue = std::stof(resetValueStr);

	float minValue = 0.0f;
	std::string minValueStr = aProperty.GetMetaDataFromKey("min");
	if (!minValueStr.empty())
		minValue = std::stof(minValueStr);

	float maxValue = 0.0f;
	std::string maxValueStr = aProperty.GetMetaDataFromKey("max");
	if (!maxValueStr.empty())
		maxValue = std::stof(maxValueStr);

	float speedValue = 0.01f;
	std::string speedValueStr = aProperty.GetMetaDataFromKey("floatdragspeed");
	if (!speedValueStr.empty())
		speedValue = std::stof(speedValueStr);

	const ImVec4 xColor = ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f };
	DrawPropertyVectorComponent("X", buttonSize, angles.x, xColor, minValue, maxValue, speedValue, resetValue);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	const ImVec4 yColor = ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f };
	DrawPropertyVectorComponent("Y", buttonSize, angles.y, yColor, minValue, maxValue, speedValue, resetValue);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	const ImVec4 zColor = ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f };
	DrawPropertyVectorComponent("Z", buttonSize, angles.z, zColor, minValue, maxValue, speedValue, resetValue);
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	quat = quat.FromEulerAngles(angles);
}

////////////////////////////////
//	Asset
void SED_PropertiesPanel::DrawPropertyInternal(SGF_PropertyHelper<SC_Ref<SR_Texture>>& aProperty) const
{
	SC_Ref<SR_Texture>& texture = aProperty.Get();
	std::string label("##_texture_");
	label += aProperty.GetName();
	ImGui::ImageButton(texture.get(), {128.f, 128.f});

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TextureDrag", ImGuiDragDropFlags_None))
		{
			SR_Texture* newTexture;
			SC_Memcpy(&newTexture, payload->Data, payload->DataSize);
			texture.reset(newTexture);
		}

		ImGui::EndDragDropTarget();
	}
}

void SED_PropertiesPanel::DrawPropertyInternal(SGF_PropertyHelper<SC_Ref<SGfx_MaterialInstance>>& aProperty) const
{
	SC_Ref<SGfx_MaterialInstance>& material = aProperty.Get();
	std::string label("##_material_");
	label += aProperty.GetName();
	ImGui::ImageButton(nullptr, { 128.f, 128.f });

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MaterialInstanceDrag", ImGuiDragDropFlags_None))
		{
			SGfx_MaterialInstance* newMaterial;
			SC_Memcpy(&newMaterial, payload->Data, payload->DataSize);
			material.reset(newMaterial);
		}

		ImGui::EndDragDropTarget();
	}
}

void SED_PropertiesPanel::DrawPropertyInternal(SGF_PropertyHelper<SC_Ref<SGfx_MeshInstance>>& aProperty) const
{
	SC_Ref<SGfx_MeshInstance>& mesh = aProperty.Get();
	std::string label("##_mesh_");
	label += aProperty.GetName();
	ImGui::ImageButton(nullptr, { 128.f, 128.f });
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MeshInstanceDrag", ImGuiDragDropFlags_None))
		{
			SGfx_MeshInstance* newMesh;
			SC_Memcpy(&newMesh, payload->Data, payload->DataSize);
			mesh.reset(newMesh);
		}

		ImGui::EndDragDropTarget();
	}
}

void SED_PropertiesPanel::DrawPropertyInternal(SGF_PropertyHelper<SC_Ref<SGF_Entity>>& aProperty) const
{
	SC_Ref<SGF_Entity>& entity = aProperty.Get();
	const char* name = aProperty.GetName();
	std::string label("##_entity_");
	label += name;

	if (mWorld)
	{
		if (entity)
			ImGui::Text("%s", entity->GetName().c_str());
		else
			ImGui::Text("<None>");

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityDrag", ImGuiDragDropFlags_None))
			{
				SGF_Entity* newEntity;
				SC_Memcpy(&newEntity, payload->Data, payload->DataSize);
				entity.reset(newEntity);
			}

			ImGui::EndDragDropTarget();
		}
	}
	else
		ImGui::Text("<None>");
}