#pragma once


void SED_PushId(const char* aId);
void SED_PushId(int aId);
void SED_PopId();

void SED_SameLine(float aOffsetX = 0.0f, float aSpacing = -1.0f);

SC_Vector2 SED_GetCurrentDrawPos();

SC_Vector2 SED_GetFramePadding();
float SED_GetCurrentFontSize();