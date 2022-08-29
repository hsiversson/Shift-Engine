#include "SGfx_MeshInstance.h"
#include "SGfx_InstanceData.h"

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
	return mMeshTemplate;
}

SGfx_MaterialInstance* SGfx_MeshInstance::GetMaterialInstance() const
{
	return mMaterialInstance;
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

void SGfx_MeshInstance::UpdateInstanceData(SGfx_InstanceData* aInstanceData)
{
	struct InstanceData
	{
		SC_Matrix mTransform;
		SC_Matrix mPrevTransform;
		uint32 mNormalBufferDescriptorIndex;
		uint32 mVertexStride;
		uint32 mIndexBufferDescriptorIndex;
		uint32 mIndexStride;
		uint32 mNumVertices;
		uint32 mVertexNormalOffset;
		uint32 mMaterialIndex;
		uint32 _pad;
	};

	InstanceData data;
	data.mTransform = mTransform;
	data.mPrevTransform = mPrevTransform;
	data.mNormalBufferDescriptorIndex = mMeshTemplate->GetNormalBuffer()->GetDescriptorHeapIndex();
	data.mVertexStride = mMeshTemplate->GetVertexBufferResource()->GetProperties().mElementSize;
	data.mIndexBufferDescriptorIndex = mMeshTemplate->GetIndexBuffer()->GetDescriptorHeapIndex();
	data.mIndexStride = mMeshTemplate->GetIndexBufferResource()->GetProperties().mElementSize;
	data.mNumVertices = mMeshTemplate->GetVertexBufferResource()->GetProperties().mElementCount;
	data.mVertexNormalOffset = mMeshTemplate->GetVertexLayout().GetAttributeByteOffset(SR_VertexAttribute::Normal);
	data.mMaterialIndex = mMaterialInstance->GetMaterialIndex();

	aInstanceData->Add(mInstanceDataOffset, sizeof(data) / sizeof(SC_Vector4), reinterpret_cast<SC_Vector4*>(&data));
}

#if SR_ENABLE_RAYTRACING
bool SGfx_MeshInstance::IncludeInRaytracingScene() const
{
	if (mMeshTemplate->GetAccelerationStructure())
		return true;

	return false;
}
SR_RaytracingInstanceProperties SGfx_MeshInstance::GetRaytracingInstanceProperties()
{
	SR_RaytracingInstanceProperties instanceProperties;
	instanceProperties.mAccelerationStructureGPUAddress = mMeshTemplate->GetAccelerationStructure()->GetGPUAddressStart();
	instanceProperties.mTransform = mTransform;
	instanceProperties.mInstanceId = mInstanceDataOffset;
	instanceProperties.mInstanceMask = 0xff;
	instanceProperties.mHitGroup = 0;
	instanceProperties.mFaceCullingMode = SR_CullMode::Back;
	instanceProperties.mIsOpaque = true;

	return instanceProperties;
}
SR_RaytracingInstanceData SGfx_MeshInstance::GetRaytracingInstanceData() const
{
	SC_ASSERT(mMaterialInstance, "No material instance available.");

	SR_RaytracingInstanceData instanceData;
	instanceData.mTransform = mTransform;
	instanceData.mMaterialIndex = mMaterialInstance->GetMaterialIndex();

	SR_Buffer* vertexBuffer = mMeshTemplate->GetVertexBuffer();
	const SR_BufferResourceProperties& vbProperties = vertexBuffer->GetResource()->GetProperties();
	const SR_VertexLayout& vertexLayout = mMeshTemplate->GetVertexLayout();

	instanceData.mVertexBufferDescriptorIndex = vertexBuffer->GetDescriptorHeapIndex();
	instanceData.mVertexStride = vbProperties.mElementSize;
	instanceData.mVertexPositionByteOffset = vertexLayout.GetAttributeByteOffset(SR_VertexAttribute::Position);
	instanceData.mVertexNormalByteOffset = vertexLayout.GetAttributeByteOffset(SR_VertexAttribute::Normal);
	instanceData.mVertexTangentByteOffset = vertexLayout.GetAttributeByteOffset(SR_VertexAttribute::Tangent);
	instanceData.mVertexBitangentByteOffset = vertexLayout.GetAttributeByteOffset(SR_VertexAttribute::Bitangent);
	instanceData.mVertexUVByteOffset = vertexLayout.GetAttributeByteOffset(SR_VertexAttribute::UV);

	SR_Buffer* indexBuffer = mMeshTemplate->GetIndexBuffer();
	instanceData.mIndexBufferDescriptorIndex = indexBuffer->GetDescriptorHeapIndex();

	return instanceData;
}
#endif

uint32 SGfx_MeshInstance::GetInstanceDataOffset() const
{
	return mInstanceDataOffset;
}

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
