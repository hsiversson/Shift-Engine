#pragma once
#include "SGfx_Mesh.h"
#include "Material/SGfx_MaterialInstance.h"
#include "RenderCore/Interface/SR_RaytracingStructs.h"

class SGfx_MeshInstance
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

#if ENABLE_RAYTRACING
	bool IncludeInRaytracingScene() const;
	const SR_RaytracingInstanceData& GetRaytracingData();
#endif
private:
	void CalculateBoundingBox();

#if ENABLE_RAYTRACING
	void UpdateRaytracingData();
#endif

private:
	SC_Matrix mTransform;
	SC_Matrix mPrevTransform;
	SC_AABB mBoundingBox;

	SC_Ref<SGfx_Mesh> mMeshTemplate;
	SC_Ref<SGfx_MaterialInstance> mMaterialInstance;

#if ENABLE_RAYTRACING
	SR_RaytracingInstanceData mRaytracingData;
#endif
};
