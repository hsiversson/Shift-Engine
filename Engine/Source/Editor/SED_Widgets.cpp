#include "SED_Widgets.h"
#include <stdarg.h>

void SED_Text(const char* aFmtStr, ...)
{
	va_list args;
	va_start(args, aFmtStr);
	ImGui::TextV(aFmtStr, args);
	va_end(args);
}

bool SED_TypeSlider(const char* aLabel, SED_DrawDataType aType, void* aVariable, uint8 aVectorSize, void* aMin, void* aMax, const char* aFormat)
{
	ImGuiDataType type;
	switch (aType)
	{
	case SED_DrawDataType::Int:
		type = ImGuiDataType_S32;
		break;
	case SED_DrawDataType::Uint:
		type = ImGuiDataType_U32;
		break;
	default:
	case SED_DrawDataType::Float:
		type = ImGuiDataType_Float;
		break;
	}

	return ImGui::SliderScalarN(aLabel, type, aVariable, aVectorSize, aMin, aMax, aFormat);
}

bool SED_TypeField(const char* aLabel, SED_DrawDataType aType, void* aVariable, uint8 aVectorSize, void* aMin, void* aMax, float aStepRate, const char* aFormat)
{
	ImGuiDataType type;
	switch (aType)
	{
	case SED_DrawDataType::Int:
		type = ImGuiDataType_S32;
		break;
	case SED_DrawDataType::Uint:
		type = ImGuiDataType_U32;
		break;
	default:
	case SED_DrawDataType::Float:
		type = ImGuiDataType_Float;
		break;
	}

	return ImGui::DragScalarN(aLabel, type, aVariable, aVectorSize, aStepRate, aMin, aMax, aFormat);
}

bool SED_FloatSlider(const char* aLabel, float& aVariable, float aMin, float aMax, const char* aFormat)
{
	return SED_TypeSlider(aLabel, SED_DrawDataType::Float, &aVariable, 1, &aMin, &aMax, aFormat);
}

bool SED_FloatSlider(const char* aLabel, SC_Vector2& aVariable, float aMin, float aMax, const char* aFormat)
{
	return SED_TypeSlider(aLabel, SED_DrawDataType::Float, &aVariable, 2, &aMin, &aMax, aFormat);
}

bool SED_FloatSlider(const char* aLabel, SC_Vector& aVariable, float aMin, float aMax, const char* aFormat)
{
	return SED_TypeSlider(aLabel, SED_DrawDataType::Float, &aVariable, 3, &aMin, &aMax, aFormat);
}

bool SED_FloatSlider(const char* aLabel, SC_Vector4& aVariable, float aMin, float aMax, const char* aFormat)
{
	return SED_TypeSlider(aLabel, SED_DrawDataType::Float, &aVariable, 4, &aMin, &aMax, aFormat);
}

bool SED_FloatField(const char* aLabel, float& aVariable, float aMin, float aMax, float aStepRate, const char* aFormat /*= "%.3f"*/)
{
	return SED_TypeField(aLabel, SED_DrawDataType::Float, &aVariable, 1, &aMin, &aMax, aStepRate, aFormat);
}

bool SED_FloatField(const char* aLabel, SC_Vector2& aVariable, float aMin, float aMax, float aStepRate, const char* aFormat)
{
	return SED_TypeField(aLabel, SED_DrawDataType::Float, &aVariable, 2, &aMin, &aMax, aStepRate, aFormat);
}

bool SED_FloatField(const char* aLabel, SC_Vector& aVariable, float aMin, float aMax, float aStepRate, const char* aFormat)
{
	return SED_TypeField(aLabel, SED_DrawDataType::Float, &aVariable, 3, &aMin, &aMax, aStepRate, aFormat);
}

bool SED_FloatField(const char* aLabel, SC_Vector4& aVariable, float aMin, float aMax, float aStepRate, const char* aFormat)
{
	return SED_TypeField(aLabel, SED_DrawDataType::Float, &aVariable, 4, &aMin, &aMax, aStepRate, aFormat);
}

bool SED_IntSlider(const char* aLabel, int& aVariable, int aMin, int aMax)
{
	return SED_TypeSlider(aLabel, SED_DrawDataType::Int, &aVariable, 1, &aMin, &aMax);
}

bool SED_IntSlider(const char* aLabel, SC_IntVector2& aVariable, int aMin, int aMax)
{
	return SED_TypeSlider(aLabel, SED_DrawDataType::Int, &aVariable, 2, &aMin, &aMax);
}

bool SED_IntSlider(const char* aLabel, SC_IntVector& aVariable, int aMin, int aMax)
{
	return SED_TypeSlider(aLabel, SED_DrawDataType::Int, &aVariable, 3, &aMin, &aMax);
}

bool SED_IntSlider(const char* aLabel, SC_IntVector4& aVariable, int aMin, int aMax)
{
	return SED_TypeSlider(aLabel, SED_DrawDataType::Int, &aVariable, 4, &aMin, &aMax);
}

bool SED_IntField(const char* aLabel, int& aVariable, int aMin, int aMax, int aStepRate)
{
	return SED_TypeField(aLabel, SED_DrawDataType::Int, &aVariable, 1, &aMin, &aMax, (float)aStepRate);
}

bool SED_IntField(const char* aLabel, SC_IntVector2& aVariable, int aMin, int aMax, int aStepRate)
{
	return SED_TypeField(aLabel, SED_DrawDataType::Int, &aVariable, 2, &aMin, &aMax, (float)aStepRate);
}

bool SED_IntField(const char* aLabel, SC_IntVector& aVariable, int aMin, int aMax, int aStepRate)
{
	return SED_TypeField(aLabel, SED_DrawDataType::Int, &aVariable, 3, &aMin, &aMax, (float)aStepRate);
}

bool SED_IntField(const char* aLabel, SC_IntVector4& aVariable, int aMin, int aMax, int aStepRate)
{
	return SED_TypeField(aLabel, SED_DrawDataType::Int, &aVariable, 4, &aMin, &aMax, (float)aStepRate);
}

bool SED_UintSlider(const char* aLabel, uint32& aVariable, uint32 aMin, uint32 aMax)
{
	return SED_TypeSlider(aLabel, SED_DrawDataType::Uint, &aVariable, 1, &aMin, &aMax);
}

bool SED_UintSlider(const char* aLabel, SC_Vector2u& aVariable, uint32 aMin, uint32 aMax)
{
	return SED_TypeSlider(aLabel, SED_DrawDataType::Uint, &aVariable, 2, &aMin, &aMax);
}

bool SED_UintSlider(const char* aLabel, SC_Vector3u& aVariable, uint32 aMin, uint32 aMax)
{
	return SED_TypeSlider(aLabel, SED_DrawDataType::Uint, &aVariable, 3, &aMin, &aMax);
}

bool SED_UintSlider(const char* aLabel, SC_Vector4u& aVariable, uint32 aMin, uint32 aMax)
{
	return SED_TypeSlider(aLabel, SED_DrawDataType::Uint, &aVariable, 4, &aMin, &aMax);
}

bool SED_UintField(const char* aLabel, uint32& aVariable, uint32 aMin, uint32 aMax, uint32 aStepRate)
{
	return SED_TypeField(aLabel, SED_DrawDataType::Uint, &aVariable, 1, &aMin, &aMax, (float)aStepRate);
}

bool SED_UintField(const char* aLabel, SC_Vector2u& aVariable, uint32 aMin, uint32 aMax, uint32 aStepRate)
{
	return SED_TypeField(aLabel, SED_DrawDataType::Uint, &aVariable, 2, &aMin, &aMax, (float)aStepRate);
}

bool SED_UintField(const char* aLabel, SC_Vector3u& aVariable, uint32 aMin, uint32 aMax, uint32 aStepRate)
{
	return SED_TypeField(aLabel, SED_DrawDataType::Uint, &aVariable, 3, &aMin, &aMax, (float)aStepRate);
}

bool SED_UintField(const char* aLabel, SC_Vector4u& aVariable, uint32 aMin, uint32 aMax, uint32 aStepRate)
{
	return SED_TypeField(aLabel, SED_DrawDataType::Uint, &aVariable, 4, &aMin, &aMax, (float)aStepRate);
}

bool SED_Checkbox(const char* aLabel, bool& aBool)
{
	return ImGui::Checkbox(aLabel, &aBool);
}

bool SED_Button(const char* aLabel, const SC_Vector2& aSize)
{
	return ImGui::Button(aLabel, aSize);
}

bool SED_ImageButton(SR_Texture* aImage, const SC_Vector2& aSize, const SC_Vector2& aMinUV, const SC_Vector2& aMaxUV)
{
	return ImGui::ImageButton(aImage, aSize, aMinUV, aMaxUV);
}

void SED_Image(SR_Texture* aImage, const SC_Vector2& aSize, const SC_Vector2& aMinUV, const SC_Vector2& aMaxUV)
{
	ImGui::Image(aImage, aSize, aMinUV, aMaxUV);
}

bool SED_ColorPickerRGB(const char* aLabel, SC_LinearColor& aColor)
{
	return ImGui::ColorPicker3(aLabel, &aColor.r, 0);
}

bool SED_ColorPickerRGBA(const char* aLabel, SC_LinearColor& aColor)
{
	return ImGui::ColorPicker4(aLabel, &aColor.r, 0);
}

bool SED_ColorPickerRGB(const char* aLabel, SC_Color& aColor)
{
	float v[3] = { (float)aColor.r, (float)aColor.g, (float)aColor.b };
	bool result = ImGui::ColorPicker3(aLabel, v, ImGuiColorEditFlags_Uint8);
	aColor.r = (uint8)v[0];
	aColor.g = (uint8)v[1];
	aColor.b = (uint8)v[2];

	return result;
}

bool SED_ColorPickerRGBA(const char* aLabel, SC_Color& aColor)
{
	float v[4] = { (float)aColor.r, (float)aColor.g, (float)aColor.b, (float)aColor.a };
	bool result = ImGui::ColorPicker4(aLabel, v, ImGuiColorEditFlags_Uint8);
	aColor.r = (uint8)v[0];
	aColor.g = (uint8)v[1];
	aColor.b = (uint8)v[2];
	aColor.a = (uint8)v[3];

	return result;
}

bool SED_ColorEditRGB(const char* aLabel, SC_LinearColor& aColor)
{
	return ImGui::ColorEdit3(aLabel, &aColor.r, 0);
}

bool SED_ColorEditRGBA(const char* aLabel, SC_LinearColor& aColor)
{
	return ImGui::ColorEdit4(aLabel, &aColor.r, 0);
}

bool SED_ColorEditRGB(const char* aLabel, SC_Color& aColor)
{
	float v[3] = { (float)aColor.r, (float)aColor.g, (float)aColor.b };
	bool result = ImGui::ColorEdit3(aLabel, v, ImGuiColorEditFlags_Uint8);
	aColor.r = (uint8)v[0];
	aColor.g = (uint8)v[1];
	aColor.b = (uint8)v[2];

	return result;
}

bool SED_ColorEditRGBA(const char* aLabel, SC_Color& aColor)
{
	float v[4] = { (float)aColor.r, (float)aColor.g, (float)aColor.b, (float)aColor.a };
	bool result = ImGui::ColorEdit4(aLabel, v, ImGuiColorEditFlags_Uint8);
	aColor.r = (uint8)v[0];
	aColor.g = (uint8)v[1];
	aColor.b = (uint8)v[2];
	aColor.a = (uint8)v[3];

	return result;
}

bool SED_Selectable(const char* aLabel, bool aSelected)
{
	return ImGui::Selectable(aLabel, &aSelected);
}

bool SED_BeginComboBox(const char* aLabel, const char* aPreviewText)
{
	return ImGui::BeginCombo(aLabel, aPreviewText, 0);
}

void SED_EndComboBox()
{
	ImGui::EndCombo();
}

bool SED_BeginMenuBar()
{
	return ImGui::BeginMenuBar();
}

void SED_EndMenuBar()
{
	ImGui::EndMenuBar();
}

bool SED_BeginMenu(const char* aLabel, bool aEnabled)
{
	return ImGui::BeginMenu(aLabel, aEnabled);
}

void SED_EndMenu()
{
	ImGui::EndMenu();
}

bool SED_MenuItem(const char* aLabel, const char* aShortcut, bool aSelected, bool aEnabled)
{
	return ImGui::MenuItem(aLabel, aShortcut, &aSelected, aEnabled);
}

bool SED_BeginDragDropTarget()
{
	return ImGui::BeginDragDropTarget();
}

void SED_EndDragDropTarget()
{
	ImGui::EndDragDropTarget();
}

bool SED_BeginDragDropSource()
{
	return ImGui::BeginDragDropSource();
}

void SED_EndDragDropSource()
{
	ImGui::EndDragDropSource();
}

bool SED_PushDragDropPayload(const char* aType, const void* aData, SC_SizeT aSize)
{
	return ImGui::SetDragDropPayload(aType, aData, aSize);
}

bool SED_RetrieveDragDropPayload(const char* aType, SED_DragDropPayload& aPayload)
{
	const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(aType, ImGuiDragDropFlags_None);
	if (payload)
	{
		aPayload.mSize = payload->DataSize;
		aPayload.mData = new uint8[aPayload.mSize];
		SC_Memcpy(aPayload.mData, payload->Data, aPayload.mSize);

		return true;
	}

	return false;
}

