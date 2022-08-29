#pragma once
#include "SGfx_Mesh.h"
#include "Graphics/Material/SGfx_MaterialInstance.h"
#include "RenderCore/Interface/SR_RaytracingStructs.h"

class SGfx_InstanceData;
class SGfx_MeshInstance : public SC_ReferenceCounted
{
public:
	static SC_Ref<SGfx_MeshInstance> Create(const SGfx_MeshCreateParams& aCreateParams);
	static SC_Ref<SGfx_MeshInstance> Create(const SC_Ref<SGfx_Mesh>& aMeshTemplate);

public:
	SGfx_MeshInstance(const SC_Ref<SGfx_Mesh>& aMeshTemplate);
	~SGfx_MeshInstance();

	void SetMaterialInstance(const SC_Ref<SGfx_MaterialInstance>& aMaterialInstance);

	SGfx_Mesh* GetMeshTemplate() const;
	SGfx_MaterialInstance* GetMaterialInstance() const;

	void SetTransform(const SC_Matrix& aTransform);
	const SC_Matrix& GetTransform() const;
	const SC_Matrix& GetPrevTransform() const;

	const SC_AABB& GetBoundingBox() const;

	void UpdateInstanceData(SGfx_InstanceData* aInstanceData);

#if SR_ENABLE_RAYTRACING
	bool IncludeInRaytracingScene() const;
	SR_RaytracingInstanceProperties GetRaytracingInstanceProperties();
	SR_RaytracingInstanceData GetRaytracingInstanceData() const;
#endif

	uint32 GetInstanceDataOffset() const;

private:
	void CalculateBoundingBox();

private:
	SC_Matrix mTransform;
	SC_Matrix mPrevTransform;
	SC_AABB mBoundingBox;

	SC_Ref<SGfx_Mesh> mMeshTemplate;
	SC_Ref<SGfx_MaterialInstance> mMaterialInstance;

	uint32 mInstanceDataOffset;
};

