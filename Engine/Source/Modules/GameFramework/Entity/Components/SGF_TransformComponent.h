#pragma once
#include "SGF_Component.h"

class SGF_TransformComponent : public SGF_Component
{
	COMPONENT_DECLARE(SGF_TransformComponent, "TransformComponent");
public:
	SGF_TransformComponent();
	~SGF_TransformComponent();

	SC_Matrix GetTransform() const;

	bool Save(SC_Json& aOutSaveData) const override;
	bool Load(const SC_Json& aSavedData) override;

	SGF_Property<SC_Vector> mPosition;
	SGF_Property<SC_Quaternion> mRotation;
	SGF_Property<SC_Vector> mScale;
};