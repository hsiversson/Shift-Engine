#pragma once
#include "GameFramework/Entity/Components/SGF_Component.h"

template<class PropertyType>
inline void DrawProperty(const char* /*aName*/, SGF_Property<PropertyType>& /*aProperty*/)
{
	return;
}

template<>
inline void DrawProperty(const char* aName, SGF_Property<SC_Vector>& aProperty)
{
	ImGui::DragFloat3(aName, &aProperty.Get().x, 0.1f);
}

template<>
inline void DrawProperty(const char* aName, SGF_Property<SC_Quaternion>& aProperty)
{
	SC_Vector angles = aProperty.Get().AsEulerAngles();

	if (ImGui::DragFloat3(aName, &angles.x, 0.1f))
	{
		aProperty.Set(aProperty.Get().FromEulerAngles(angles));
	}
}