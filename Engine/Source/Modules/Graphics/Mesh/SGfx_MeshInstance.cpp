#include "SGfx_MeshInstance.h"

SC_Ref<SGfx_MeshInstance> SGfx_MeshInstance::Create(const SGfx_MeshCreateParams& aCreateParams)
{
	SC_Ref<SGfx_Mesh> meshTemplate = SGfx_Mesh::Create(aCreateParams);
	if (!meshTemplate)
		return nullptr;

	return SC_MakeRef<SGfx_MeshInstance>(meshTemplate);
}

SC_Ref<SGfx_MeshInstance> SGfx_MeshInstance::Create(const SC_Ref<SGfx_Mesh>& aMeshTemplate)
{
	return SC_MakeRef<SGfx_MeshInstance>(aMeshTemplate);
}

SGfx_MeshInstance::SGfx_MeshInstance(const SC_Ref<SGfx_Mesh>& aMeshTemplate)
	: mTransform(SC_Matrix::Identity())
	, mPrevTransform(SC_Matrix::Identity())
	, mMeshTemplate(aMeshTemplate)
{
	CalculateBoundingBox();
}

SGfx_MeshInstance::~SGfx_MeshInstance()
{

}

void SGfx_MeshInstance::SetMaterialInstance(const SC_Ref<SGfx_MaterialInstance>& aMaterialInstance)
{
	mMaterialInstance = aMaterialInstance;
}

SGfx_Mesh* SGfx_MeshInstance::GetMeshTemplate() const
{
	return mMeshTemplate.get();
}

SGfx_MaterialInstance* SGfx_MeshInstance::GetMaterialInstance() const
{
	return mMaterialInstance.get();
}

void SGfx_MeshInstance::SetTransform(const SC_Matrix& aTransform)
{
	mPrevTransform = mTransform;
	mTransform = aTransform;
	CalculateBoundingBox();
}

const SC_Matrix& SGfx_MeshInstance::GetTransform() const
{
	return mTransform;
}

const SC_Matrix& SGfx_MeshInstance::GetPrevTransform() const
{
	return mPrevTransform;
}

const SC_AABB& SGfx_MeshInstance::GetBoundingBox() const
{
	return mBoundingBox;
}

void SGfx_MeshInstance::UpdateInstanceData() const
{
	struct InstanceData
	{
		SC_Matrix mTransform;
		SC_Matrix mPrevTransform;
		uint32 mVertexBufferDescriptorIndex;
		uint32 mVertexStride;
		uint32 mNumVertices;
		uint32 mVertexNormalOffset;
		uint32 mMaterialIndex;
	};

	InstanceData data;
	data.mTransform = mTransform;
	data.mPrevTransform = mPrevTransform;
	data.mVertexBufferDescriptorIndex = mMeshTemplate->GetVertexBuffer()->GetDescriptorHeapIndex();
	data.mVertexStride = mMeshTemplate->GetVertexBufferResource()->GetProperties().mElementSize;
	data.mNumVertices = mMeshTemplate->GetVertexBufferResource()->GetProperties().mElementCount;
	data.mVertexNormalOffset = mMeshTemplate->GetVertexLayout().GetAttributeByteOffset(SR_VertexAttribute::Normal);
	data.mMaterialIndex = mMaterialInstance->GetMaterialIndex();

}

#if ENABLE_RAYTRACING
bool SGfx_MeshInstance::IncludeInRaytracingScene() const
{
	if (mMeshTemplate->GetAccelerationStructure())
		return true;

	return false;
}
const SR_RaytracingInstanceData& SGfx_MeshInstance::GetRaytracingData()
{
	UpdateRaytracingData();
	return mRaytracingData;
}
#endif

void SGfx_MeshInstance::CalculateBoundingBox()
{
	const SC_AABB& templateBoundingBox = mMeshTemplate->GetBoundingBox();
	mBoundingBox.mMin = templateBoundingBox.mMin;
	mBoundingBox.mMax = templateBoundingBox.mMax;

	SC_Vector corners[8];

	// Near face
	corners[0] = SC_Vector(mBoundingBox.mMin) * mTransform;
	corners[1] = SC_Vector(mBoundingBox.mMin.x, mBoundingBox.mMax.y, mBoundingBox.mMin.z) * mTransform;
	corners[2] = SC_Vector(mBoundingBox.mMax.x, mBoundingBox.mMax.y, mBoundingBox.mMin.z) * mTransform;
	corners[3] = SC_Vector(mBoundingBox.mMax.x, mBoundingBox.mMin.y, mBoundingBox.mMin.z) * mTransform;

	// Far face
	corners[4] = SC_Vector(mBoundingBox.mMin.x, mBoundingBox.mMin.y, mBoundingBox.mMax.z) * mTransform;
	corners[5] = SC_Vector(mBoundingBox.mMin.x, mBoundingBox.mMax.y, mBoundingBox.mMax.z) * mTransform;
	corners[6] = SC_Vector(mBoundingBox.mMax) * mTransform;
	corners[7] = SC_Vector(mBoundingBox.mMax.x, mBoundingBox.mMin.y, mBoundingBox.mMax.z) * mTransform;

	mBoundingBox.FromPoints(corners, 8);
}

#if ENABLE_RAYTRACING
void SGfx_MeshInstance::UpdateRaytracingData()
{
	mRaytracingData.mAccelerationStructureGPUAddress = mMeshTemplate->GetAccelerationStructure()->GetGPUAddressStart();
	mRaytracingData.mTransform = mTransform;
	mRaytracingData.mInstanceId = 0;
	mRaytracingData.mInstanceMask = 0xff;
	mRaytracingData.mHitGroup = 0;
	mRaytracingData.mFaceCullingMode = SR_CullMode::Back;
	mRaytracingData.mIsOpaque = true;



	mMeshTemplate->GetVertexBuffer()->GetDescriptorHeapIndex();

}
#endif
