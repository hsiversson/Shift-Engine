#pragma once
#include "SGF_Component.h"

class SGfx_MeshInstance;
class SGfx_MaterialInstance;
class SGF_StaticMeshComponent : public SGF_Component
{
	COMPONENT_DECLARE(SGF_StaticMeshComponent, "StaticMeshComponent");
public:
	SGF_StaticMeshComponent();
	~SGF_StaticMeshComponent();

	void OnUpdate() override;

	void SetMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance);
	void SetMaterial(SC_Ref<SGfx_MaterialInstance> aMaterialInstance);

	void SetVisible(bool aValue);

	bool Save(SC_Json& aOutSaveData) const override;
	bool Load(const SC_Json& aSavedData) override;
private:

	// Support multiple meshes?
	SC_Ref<SGfx_MeshInstance> mMeshInstance;
	SC_Ref<SGfx_MaterialInstance> mMaterialInstance;

	bool mIsVisible;
};