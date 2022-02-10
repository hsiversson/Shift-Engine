#pragma once
#include "Common/SC_Base.h"

#define IM_VEC2_CLASS_EXTRA                                                                     \
        ImVec2(const SC_Vector2& f) { x = f.x; y = f.y; }                                       \
        operator SC_Vector2() const { return SC_Vector2(x,y); }                                 \
        ImVec2 operator+(const ImVec2& aOther) { return ImVec2(x + aOther.x, y + aOther.y);}

#define IM_VEC4_CLASS_EXTRA                                                                                                 \
        ImVec4(const SC_Vector4& f) { x = f.x; y = f.y; z = f.z; w = f.w; }                                                 \
        operator SC_Vector4() const { return SC_Vector4(x,y,z,w); }                                                         \
        ImVec4 operator+(const ImVec4& aOther) { return ImVec4(x + aOther.x, y + aOther.y, z + aOther.z, w + aOther.w);}