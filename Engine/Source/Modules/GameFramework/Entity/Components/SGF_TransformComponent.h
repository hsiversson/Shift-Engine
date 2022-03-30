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

	SGF_PROPERTY(SC_Vector, mPosition, "Position", SC_Vector::ZeroVector());
	SGF_PROPERTY(SC_Quaternion, mRotation, "Rotation", SC_Quaternion::Identity());
	SGF_PROPERTY(SC_Vector, mScale, "Scale", SC_Vector::OneVector());
	SGF_PROPERTY(bool, mIsStatic, "Static");
};