#pragma once
#include "RenderCore/Interface/SR_VertexLayout.h"
#include "SGfx_Meshlet.h"

class SC_AABB;
class SR_BufferResource;
class SR_Buffer;
class SGfx_MaterialInstance;

struct SGfx_MeshCreateParams
{
	SGfx_MeshCreateParams() : mIndexStride(0), mVertexIndicesStride(0), mIsMeshletData(false) {}

	SC_FilePath mSourceFile;

	SC_Vector mAABBMin;
	SC_Vector mAABBMax;

	SR_VertexLayout mVertexLayout;
	SC_Array<uint8> mVertexData;

	SC_Array<uint8> mIndexData;
	uint32 mIndexStride;

	SC_Array<SGfx_Meshlet> mMeshlets;
	SC_Array<SGfx_PackedPrimitiveTriangle> mPrimitiveIndices;
	SC_Array<uint8> mVertexIndices;
	uint32 mVertexIndicesStride;

	bool mIsMeshletData;
};

struct SGfx_MeshletBuffers
{
	SC_Ref<SR_Buffer> mVertexBuffer;
	SC_Ref<SR_Buffer> mMeshletBuffer;
	SC_Ref<SR_Buffer> mVertexIndexBuffer;
	SC_Ref<SR_Buffer> mPrimitiveIndexBuffer;
};

class SGfx_Mesh : public SC_ReferenceCounted
{
	friend class SGfx_MeshCache;
public:
	static SC_Ref<SGfx_Mesh> Create(const SGfx_MeshCreateParams& aCreateParams);

public:
	SGfx_Mesh();
	~SGfx_Mesh();

	const SC_AABB& GetBoundingBox() const;

	const SR_VertexLayout& GetVertexLayout() const;

	SR_BufferResource* GetVertexBufferResource() const;
	SR_Buffer* GetVertexBuffer() const;
	SR_Buffer* GetNormalBuffer() const;
	SR_BufferResource* GetIndexBufferResource() const;
	SR_Buffer* GetIndexBuffer() const;

#if SR_ENABLE_MESH_SHADERS
	const SGfx_MeshletBuffers& GetMeshletBuffers() const;
	bool IsUsingMeshlets() const;
#endif

#if SR_ENABLE_RAYTRACING
	SR_BufferResource* GetAccelerationStructure() const;
#endif

	const SC_FilePath& GetSourceFile() const;

private:
	bool InitDefault(const SGfx_MeshCreateParams& aCreateParams);
#if SR_ENABLE_MESH_SHADERS
	bool InitForMeshShaders(const SGfx_MeshCreateParams& aCreateParams);
#endif

#if SR_ENABLE_RAYTRACING
	bool InitAccelerationStructure(const SGfx_MeshCreateParams& aCreateParams);
#endif

private:
	bool Init(const SGfx_MeshCreateParams& aCreateParams);

private:
	SC_FilePath mSourceFile;
	SC_AABB mBoundingBox;
	SR_VertexLayout mVertexLayout;

	SC_Ref<SR_BufferResource> mVertexBufferResource;
	SC_Ref<SR_Buffer> mVertexBuffer;
	SC_Ref<SR_Buffer> mVertexNormalBuffer;
	SC_Ref<SR_BufferResource> mIndexBufferResource;
	SC_Ref<SR_Buffer> mIndexBuffer;
#if SR_ENABLE_MESH_SHADERS
	SGfx_MeshletBuffers mMeshletBuffers;
#endif
#if SR_ENABLE_RAYTRACING
	SC_Ref<SR_BufferResource> mAccelerationStructure;
#endif

	bool mIsMeshletMesh;
	bool mUsingMeshlets;
};

