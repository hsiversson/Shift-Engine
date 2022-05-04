#include "SGfx_Mesh.h"
#include "SGfx_MeshletGenerator.h"

class SGfx_MeshCache
{
public:
	static bool Exists(const SC_FilePath& aFilePath)
	{
		SGfx_MeshCache* instance = GetInstance();
		return instance->mCache.find(std::string(aFilePath.GetStr())) != instance->mCache.end();
	}
	static SC_Ref<SGfx_Mesh> Get(const SC_FilePath& aFilePath)
	{
		SGfx_MeshCache* instance = GetInstance();
		return instance->mCache.at(std::string(aFilePath.GetStr()));
	}
	static SC_Ref<SGfx_Mesh> Create(const SGfx_MeshCreateParams& aCreateParams)
	{
		SGfx_MeshCache* instance = GetInstance();

		SC_Ref<SGfx_Mesh> mesh = SC_MakeRef<SGfx_Mesh>();
		if (!mesh->Init(aCreateParams))
			return nullptr;

		instance->mCache.insert(std::pair(std::string(aCreateParams.mSourceFile.GetStr()), mesh));
		return mesh;
	}


private:
	SGfx_MeshCache() {}
	~SGfx_MeshCache() {}
	static void CreateInstance()
	{
		if (!gInstance)
			gInstance = new SGfx_MeshCache;
	}

	static void DestroyInstance()
	{
		delete gInstance;
		gInstance = nullptr;
	}
	static SGfx_MeshCache* GetInstance() 
	{ 
		if (!gInstance) 
			CreateInstance();

		return gInstance; 
	}

	static SGfx_MeshCache* gInstance;
	SC_UnorderedMap<std::string, SC_Ref<SGfx_Mesh>> mCache;
};
SGfx_MeshCache* SGfx_MeshCache::gInstance = nullptr;

SGfx_Mesh::SGfx_Mesh()
	: mIsMeshletMesh(false)
	, mUsingMeshlets(false)
{

}

SGfx_Mesh::~SGfx_Mesh()
{

}

bool SGfx_Mesh::Init(const SGfx_MeshCreateParams& aCreateParams)
{
	SR_BufferResourceProperties resourceProps;
	resourceProps.mBindFlags = SR_BufferBindFlag_VertexBuffer | SR_BufferBindFlag_Buffer;
	resourceProps.mElementSize = aCreateParams.mVertexLayout.GetVertexStrideSize();
	resourceProps.mElementCount = aCreateParams.mVertexData.Count() / resourceProps.mElementSize;
	resourceProps.mWritable = false;

	if (mVertexBufferResource = SR_RenderDevice::gInstance->CreateBufferResource(resourceProps, aCreateParams.mVertexData.GetBuffer()))
	{
		SR_BufferProperties bufferProps;
		bufferProps.mElementCount = mVertexBufferResource->GetProperties().mElementCount;
		bufferProps.mType = SR_BufferType::Structured;
		mVertexBuffer = SR_RenderDevice::gInstance->CreateBuffer(bufferProps, mVertexBufferResource);

#if ENABLE_MESH_SHADERS
		if (SR_RenderDevice::gInstance->GetSupportCaps().mEnableMeshShaders && aCreateParams.mIsMeshletData)
		{
			if (!InitForMeshShaders(aCreateParams))
			{
				SC_ERROR("Couldn't init mesh for Mesh Shader pipeline.");
				return false;
			}
		}
		else
#endif
		{
			if (!InitDefault(aCreateParams))
			{
				SC_ERROR("Couldn't init mesh for Default geometry pipeline.");
				return false;
			}
		}
		mIsMeshletMesh = aCreateParams.mIsMeshletData;

#if ENABLE_RAYTRACING
		//if (aCreateParams.mIncludeInRaytracing)
		{
			if (!InitAccelerationStructure(aCreateParams))
			{
				SC_ERROR("Couldn't create mesh acceleration structure.");
				return false;
			}
		}
#endif

		mVertexLayout = aCreateParams.mVertexLayout;
		mBoundingBox.mMin = aCreateParams.mAABBMin;
		mBoundingBox.mMax = aCreateParams.mAABBMax;
		mSourceFile = aCreateParams.mSourceFile;
	}
	else
		return false;

	return true;
}

SC_Ref<SGfx_Mesh> SGfx_Mesh::Create(const SGfx_MeshCreateParams& aCreateParams)
{
	if (SGfx_MeshCache::Exists(aCreateParams.mSourceFile))
		return SGfx_MeshCache::Get(aCreateParams.mSourceFile);

	return SGfx_MeshCache::Create(aCreateParams);
}

const SC_AABB& SGfx_Mesh::GetBoundingBox() const
{
	return mBoundingBox;
}

const SR_VertexLayout& SGfx_Mesh::GetVertexLayout() const
{
	return mVertexLayout;
}

SR_BufferResource* SGfx_Mesh::GetVertexBufferResource() const
{
	return mVertexBufferResource.get();
}

SR_Buffer* SGfx_Mesh::GetVertexBuffer() const
{
	return mVertexBuffer.get();
}

SR_BufferResource* SGfx_Mesh::GetIndexBufferResource() const
{
	return mIndexBufferResource.get();
}

#if ENABLE_MESH_SHADERS
const SGfx_MeshletBuffers& SGfx_Mesh::GetMeshletBuffers() const
{
	return mMeshletBuffers;
}
bool SGfx_Mesh::IsUsingMeshlets() const
{
	return mUsingMeshlets;
}
#endif

bool SGfx_Mesh::InitDefault(const SGfx_MeshCreateParams& aCreateParams)
{
	SR_BufferResourceProperties resourceProps;
	resourceProps.mBindFlags = SR_BufferBindFlag_IndexBuffer | SR_BufferBindFlag_Buffer;
	resourceProps.mWritable = false;

	const uint8* indexData = nullptr;
	if (aCreateParams.mIsMeshletData)
	{
		SC_Array<uint8> indices;
		uint32 indexStride;
		SGfx_FlattenMeshletPrimitivesToIndexBuffer(aCreateParams.mMeshlets, aCreateParams.mPrimitiveIndices, aCreateParams.mVertexIndices, aCreateParams.mVertexIndicesStride, indices, indexStride);

		resourceProps.mElementCount = indices.Count() * indexStride;
		resourceProps.mElementSize = indexStride;
		indexData = indices.GetBuffer();
	}
	else
	{
		resourceProps.mElementCount = aCreateParams.mIndexData.Count() * aCreateParams.mIndexStride;
		resourceProps.mElementSize = aCreateParams.mIndexStride;
		indexData = aCreateParams.mIndexData.GetBuffer();
	}

	mIndexBufferResource = SR_RenderDevice::gInstance->CreateBufferResource(resourceProps, indexData);

	return (mIndexBufferResource != nullptr);
}

#if ENABLE_MESH_SHADERS
bool SGfx_Mesh::InitForMeshShaders(const SGfx_MeshCreateParams& aCreateParams)
{
	SR_BufferProperties bufferProps;
	bufferProps.mElementCount = mVertexBufferResource->GetProperties().mElementCount;
	bufferProps.mType = SR_BufferType::Structured;
	mMeshletBuffers.mVertexBuffer = SR_RenderDevice::gInstance->CreateBuffer(bufferProps, mVertexBufferResource);

	SR_BufferResourceProperties bufferResourceProps;
	bufferResourceProps.mBindFlags = SR_BufferBindFlag_Buffer;
	bufferResourceProps.mElementCount = aCreateParams.mMeshlets.Count();
	bufferResourceProps.mElementSize = aCreateParams.mMeshlets.ElementStride();
	if (SC_Ref<SR_BufferResource> meshletBuffer = SR_RenderDevice::gInstance->CreateBufferResource(bufferResourceProps, aCreateParams.mMeshlets.GetBuffer()))
	{
		bufferProps.mElementCount = aCreateParams.mMeshlets.Count();
		mMeshletBuffers.mMeshletBuffer = SR_RenderDevice::gInstance->CreateBuffer(bufferProps, meshletBuffer);
	}
	else
		return false;

	bufferResourceProps.mElementCount = aCreateParams.mPrimitiveIndices.Count();
	bufferResourceProps.mElementSize = aCreateParams.mPrimitiveIndices.ElementStride();
	if (SC_Ref<SR_BufferResource> primitiveIndexBuffer = SR_RenderDevice::gInstance->CreateBufferResource(bufferResourceProps, aCreateParams.mPrimitiveIndices.GetBuffer()))
	{
		bufferProps.mElementCount = aCreateParams.mPrimitiveIndices.Count();
		mMeshletBuffers.mPrimitiveIndexBuffer = SR_RenderDevice::gInstance->CreateBuffer(bufferProps, primitiveIndexBuffer);
	}
	else
		return false;

	bufferResourceProps.mElementCount = aCreateParams.mVertexIndices.Count() / aCreateParams.mVertexIndicesStride;
	bufferResourceProps.mElementSize = aCreateParams.mVertexIndicesStride;
	if (SC_Ref<SR_BufferResource> vertexIndexBuffer = SR_RenderDevice::gInstance->CreateBufferResource(bufferResourceProps, aCreateParams.mVertexIndices.GetBuffer()))
	{
		bufferProps.mType = SR_BufferType::Default;
		bufferProps.mFormat = (aCreateParams.mVertexIndicesStride == sizeof(uint16)) ? SR_Format::R16_UINT : SR_Format::R32_UINT;
		bufferProps.mElementCount = bufferResourceProps.mElementCount;
		mMeshletBuffers.mVertexIndexBuffer = SR_RenderDevice::gInstance->CreateBuffer(bufferProps, vertexIndexBuffer);
	}
	else
		return false;

	mUsingMeshlets = true;
	return true;
}
#endif

#if ENABLE_RAYTRACING
bool SGfx_Mesh::InitAccelerationStructure(const SGfx_MeshCreateParams& aCreateParams)
{
	SC_Array<SR_RaytracingGeometryData> geometryData;
	SR_RaytracingGeometryData& geometry = geometryData.Add();

	geometry.mVertexFormat = SR_Format::RGB32_FLOAT;
	geometry.mVertexBuffer = mVertexBufferResource.get();

	SC_Ref<SR_BufferResource> indexBuffer;
	if (mIsMeshletMesh)
	{
		SC_Array<uint8> indices;
		uint32 indexStride;
		SGfx_FlattenMeshletPrimitivesToIndexBuffer(aCreateParams.mMeshlets, aCreateParams.mPrimitiveIndices, aCreateParams.mVertexIndices, aCreateParams.mVertexIndicesStride, indices, indexStride);

		SR_BufferResourceProperties indexBufferProps;
		indexBufferProps.mBindFlags = SR_BufferBindFlag_IndexBuffer;
		indexBufferProps.mElementCount = indices.Count() / indexStride;
		indexBufferProps.mElementSize = indexStride;

		// re-generate index buffer from meshlets if needed
		indexBuffer = SR_RenderDevice::gInstance->CreateBufferResource(indexBufferProps, indices.GetBuffer());
		geometry.mIndexBuffer = indexBuffer.get();
	}
	else
		geometry.mIndexBuffer = mIndexBufferResource.get();

	SR_AccelerationStructureInputs inputs;
	inputs.mIsTopLevel = false;
	inputs.mGeometryData = &geometryData;

	//if (aFastUpdate)
	//{
	//	//uint32 flags = SR_AccelerationStructureInputs::BuildFlag_AllowUpdate | SR_AccelerationStructureInputs::BuildFlag_OptimizeBuildSpeed;
	//	//if (aBuffer)
	//	//	flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
	//
	//	//inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS(flags);
	//}
	//else
	inputs.mFlags |= SR_AccelerationStructureInputs::BuildFlag_OptimizeTraceSpeed;

	auto UploadData = [this, &inputs]()
	{
		SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
		mAccelerationStructure = cmdList->CreateAccelerationStructure(inputs, nullptr);
	};

	SC_Ref<SR_TaskEvent> taskEvent = SC_MakeRef<SR_TaskEvent>();
	SR_RenderDevice::gInstance->GetCommandQueueManager()->SubmitTask(UploadData, SR_CommandListType::Graphics, taskEvent.get());

	taskEvent->mCPUEvent.Wait();
	taskEvent->mFence.Wait();

	return (mAccelerationStructure != nullptr);
}
#endif

#if ENABLE_RAYTRACING
SR_BufferResource* SGfx_Mesh::GetAccelerationStructure() const
{
	return mAccelerationStructure.get();
}
#endif

const SC_FilePath& SGfx_Mesh::GetSourceFile() const
{
	return mSourceFile;
}
