#include "SED_PropertyDrawDeclarations.h"
#include "Graphics/Material/SGfx_MaterialInstance.h"
#include "Graphics/Mesh/SGfx_MeshInstance.h"
#include "RenderCore/Interface/SR_Texture.h"
#include "imgui_internal.h"

void DrawProperty(const char* aName, bool& aProperty);
void DrawProperty(const char* aName, int32& aProperty);
void DrawProperty(const char* aName, uint32& aProperty);
void DrawProperty(const char* aName, float& aProperty);
void DrawProperty(const char* aName, SC_Vector& aProperty);
void DrawProperty(const char* aName, SC_Quaternion& aProperty);
void DrawProperty(const char* aName, SC_Ref<SR_Texture>& aProperty);
void DrawProperty(const char* aName, SC_Ref<SGfx_MaterialInstance>& aProperty);
void DrawProperty(const char* aName, SC_Ref<SGfx_MeshInstance>& aProperty);

void SED_DrawProperty(const SGF_PropertyBase::Type& aType, void* aData, const char* aPropertyName)
{
	switch (aType)
	{
	case SGF_PropertyBase::Type::Bool:
	{
		bool* boolPtr = static_cast<bool*>(aData);
		DrawProperty(aPropertyName, *boolPtr);
		break;
	}
	case SGF_PropertyBase::Type::Int:
	{
		int32* intPtr = static_cast<int32*>(aData);
		DrawProperty(aPropertyName, *intPtr);
		break;
	}
	case SGF_PropertyBase::Type::Uint:
	{
		uint32* uintPtr = static_cast<uint32*>(aData);
		DrawProperty(aPropertyName, *uintPtr);
		break;
	}
	case SGF_PropertyBase::Type::Float:
	{
		float* floatPtr = static_cast<float*>(aData);
		DrawProperty(aPropertyName, *floatPtr);
		break;
	}
	case SGF_PropertyBase::Type::Vector:
	{
		SC_Vector* vectorPtr = static_cast<SC_Vector*>(aData);
		DrawProperty(aPropertyName, *vectorPtr);
		break;
	}
	case SGF_PropertyBase::Type::Quaternion:
	{
		SC_Quaternion* quatPtr = static_cast<SC_Quaternion*>(aData);
		DrawProperty(aPropertyName, *quatPtr);
		break;
	}
	case SGF_PropertyBase::Type::Texture:
	{
		SC_Ref<SR_Texture>* texPtr = static_cast<SC_Ref<SR_Texture>*>(aData);
		DrawProperty(aPropertyName, *texPtr);
		break;
	}
	case SGF_PropertyBase::Type::Material:
	{
		SC_Ref<SGfx_MaterialInstance>* matPtr = static_cast<SC_Ref<SGfx_MaterialInstance>*>(aData);
		DrawProperty(aPropertyName, *matPtr);
		break;
	}
	case SGF_PropertyBase::Type::Mesh:
	{
		SC_Ref<SGfx_MeshInstance>* meshPtr = static_cast<SC_Ref<SGfx_MeshInstance>*>(aData);
		DrawProperty(aPropertyName, *meshPtr);
		break;
	}
	}
}
////////////////////////////////
// Native Types
void DrawProperty(const char* aName, bool& aProperty)
{
	ImGui::PushID(aName);
	ImGui::Text(aName);
	ImGui::NextColumn();

	std::string label("##_checkbox_");
	label += aName;
	ImGui::Checkbox(label.c_str(), &aProperty);
	ImGui::NextColumn();
	ImGui::PopID();
}

void DrawProperty(const char* aName, int32& aProperty)
{
	ImGui::PushID(aName);
	ImGui::Text(aName);
	ImGui::NextColumn();

	std::string label("##_checkbox_");
	label += aName;
	ImGui::DragInt(label.c_str(), &aProperty);
	ImGui::NextColumn();
	ImGui::PopID();
}

void DrawProperty(const char* aName, uint32& aProperty)
{
	ImGui::PushID(aName);
	ImGui::Text(aName);
	ImGui::NextColumn();

	std::string label("##_checkbox_");
	label += aName;
	ImGui::DragScalar(label.c_str(), ImGuiDataType_U32, &aProperty);
	ImGui::NextColumn();
	ImGui::PopID();
}

void DrawProperty(const char* aName, float& aProperty)
{
	ImGui::PushID(aName);
	ImGui::Text(aName);
	ImGui::NextColumn();

	std::string label("##_checkbox_");
	label += aName;
	ImGui::DragFloat(label.c_str(), &aProperty);
	ImGui::NextColumn();
	ImGui::PopID();
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
	ImGui::PopItemWidth();
	ImGui::PopStyleVar();
}

void DrawProperty(const char* aName, SC_Vector& aProperty)
{
	ImGui::PushID(aName);
	ImGui::Text(aName);
	ImGui::NextColumn();
	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 4, 6 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 5.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	const ImVec4 xColor = ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f };
	DrawPropertyVectorComponent("X", buttonSize, aProperty.x, xColor, 0);
	ImGui::SameLine();

	const ImVec4 yColor = ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f };
	DrawPropertyVectorComponent("Y", buttonSize, aProperty.y, yColor, 0);
	ImGui::SameLine();

	const ImVec4 zColor = ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f };
	DrawPropertyVectorComponent("Z", buttonSize, aProperty.z, zColor, 0);

	ImGui::PopStyleVar();
	ImGui::NextColumn();
	ImGui::PopID();
}

////////////////////////////////
//	Quaternion / Rotation
void DrawProperty(const char* aName, SC_Quaternion& aProperty)
{
	SC_Vector anglesProperty = aProperty.AsEulerAngles();
	DrawProperty(aName, anglesProperty);
	aProperty.FromEulerAngles(anglesProperty);
}

////////////////////////////////
//	Asset
void DrawProperty(const char* aName, SC_Ref<SR_Texture>& aProperty)
{
	ImGui::PushID(aName);
	ImGui::Text(aName);
	ImGui::NextColumn();

	std::string label("##_texture_");
	label += aName;
	ImGui::ImageButton(aProperty.get(), { 128.f, 128.f });
	ImGui::NextColumn();
	ImGui::PopID();
}

void DrawProperty(const char* aName, SC_Ref<SGfx_MaterialInstance>& /*aProperty*/)
{
	ImGui::PushID(aName);
	ImGui::Text(aName);
	ImGui::NextColumn();

	std::string label("##_material_");
	label += aName;
	ImGui::ImageButton(nullptr, { 128.f, 128.f });
	ImGui::NextColumn();
	ImGui::PopID();
}

void DrawProperty(const char* aName, SC_Ref<SGfx_MeshInstance>& /*aProperty*/)
{
	ImGui::PushID(aName);
	ImGui::Text(aName);
	ImGui::NextColumn();

	std::string label("##_mesh_");
	label += aName;
	ImGui::ImageButton(nullptr, { 128.f, 128.f });
	ImGui::NextColumn();
	ImGui::PopID();
}