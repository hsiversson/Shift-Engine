#pragma once
#include "SGF_Component.h"

class SGfx_MeshInstance;
class SGfx_MaterialInstance;
class SGF_StaticMeshComponent : public SGF_Component
{
	COMPONENT_DECLARE(SGF_StaticMeshComponent, "StaticMesh");
public:
	SGF_StaticMeshComponent();
	~SGF_StaticMeshComponent();

	void OnCreate() override;
	void OnUpdate() override;

	void SetMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance);
	void SetMaterial(SC_Ref<SGfx_MaterialInstance> aMaterialInstance);

	void SetVisible(bool aValue);

	SC_Ref<SGfx_MeshInstance>& GetMeshInstance();
	SC_Ref<SGfx_MaterialInstance>& GetMaterialInstance();

	bool Save(SC_Json& aOutSaveData) const override;
	bool Load(const SC_Json& aSavedData) override;
private:

	// Support multiple meshes?
	SGF_PROPERTY(SC_Ref<SGfx_MeshInstance>, mMeshInstance, "Mesh");
	SGF_PROPERTY(SC_Ref<SGfx_MaterialInstance>, mMaterialInstance, "Material");
	SGF_PROPERTY(SC_Ref<SR_Texture>, mTexture, "Texture");

	bool mIsVisible;
};