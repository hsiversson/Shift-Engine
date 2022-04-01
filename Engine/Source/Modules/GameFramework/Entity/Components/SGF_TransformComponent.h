#pragma once
#include "SGF_Component.h"

class SGF_TransformComponent : public SGF_Component
{
	COMPONENT_DECLARE(SGF_TransformComponent, "Transform");
public:
	SGF_TransformComponent();
	~SGF_TransformComponent();

	SC_Matrix GetTransform() const;

	bool Save(SC_Json& aOutSaveData) const override;
	bool Load(const SC_Json& aSavedData) override;

	SGF_PROPERTY(SC_Vector, mPosition, "Position", "resetvalue=0");
	SGF_PROPERTY(SC_Quaternion, mRotation, "Rotation", "resetvalue=0");
	SGF_PROPERTY(SC_Vector, mScale, "Scale", "resetvalue=1");
	SGF_PROPERTY(bool, mIsStatic, "Static");
};