#include "SED_WidgetUtils.h"

#include "imgui_internal.h"

void SED_PushId(const char* aId)
{
	ImGui::PushID(aId);
}

void SED_PushId(int aId)
{
	ImGui::PushID(aId);
}

void SED_PopId()
{
	ImGui::PopID();
}

void SED_SameLine(float aOffsetX, float aSpacing)
{
	ImGui::SameLine(aOffsetX, aSpacing);
}

SC_Vector2 SED_GetCurrentDrawPos()
{
	return ImGui::GetCursorScreenPos();
}

SC_Vector2 SED_GetFramePadding()
{
	const ImGuiStyle& style = ImGui::GetStyle();
	return style.FramePadding;
}

float SED_GetCurrentFontSize()
{
	const ImGuiContext& context = *ImGui::GetCurrentContext();
	return context.FontSize;
}
