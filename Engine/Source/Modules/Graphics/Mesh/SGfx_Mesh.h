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
	SGfx_MeshCreateParams(const SGfx_MeshCreateParams& aOther) { *this = aOther;}
	SGfx_MeshCreateParams(SGfx_MeshCreateParams&& aOther) { *this = SC_Move(aOther);}
	void operator=(const SGfx_MeshCreateParams& aOther)
	{
		mSourceFile = aOther.mSourceFile;
		mAABBMin = aOther.mAABBMin;
		mAABBMax = aOther.mAABBMax;
		mVertexLayout = aOther.mVertexLayout;
		mVertexData = aOther.mVertexData;
		mIndexData = aOther.mIndexData;
		mIndexStride = aOther.mIndexStride;
		mMeshlets = aOther.mMeshlets;
		mPrimitiveIndices = aOther.mPrimitiveIndices;
		mVertexIndices = aOther.mVertexIndices;
		mVertexIndicesStride = aOther.mVertexIndicesStride;
		mIsMeshletData = aOther.mIsMeshletData;
	}

	void operator=(SGfx_MeshCreateParams&& aOther)
	{
		mSourceFile = aOther.mSourceFile;
		mAABBMin = aOther.mAABBMin;
		mAABBMax = aOther.mAABBMax;
		mVertexLayout = aOther.mVertexLayout;
		mVertexData.Swap(aOther.mVertexData);
		mIndexData.Swap(aOther.mIndexData);
		mIndexStride = aOther.mIndexStride;
		mMeshlets = aOther.mMeshlets;
		mPrimitiveIndices = aOther.mPrimitiveIndices;
		mVertexIndices = aOther.mVertexIndices;
		mVertexIndicesStride = aOther.mVertexIndicesStride;
		mIsMeshletData = aOther.mIsMeshletData;

		aOther.mSourceFile = SC_FilePath();
		aOther.mAABBMin = SC_Vector();
		aOther.mAABBMax = SC_Vector();
		aOther.mVertexLayout = SR_VertexLayout();
		aOther.mVertexData.Reset();
		aOther.mIndexData.Reset();
		aOther.mIndexStride = 0;
		aOther.mMeshlets.Reset();
		aOther.mPrimitiveIndices.Reset();
		aOther.mVertexIndices.Reset();
		aOther.mVertexIndicesStride = 0;
		aOther.mIsMeshletData = 0;
	}

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

class SGfx_Mesh
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
	SR_BufferResource* GetIndexBufferResource() const;

#if ENABLE_MESH_SHADERS
	const SGfx_MeshletBuffers& GetMeshletBuffers() const;
	bool IsUsingMeshlets() const;
#endif

#if ENABLE_RAYTRACING
	SR_BufferResource* GetAccelerationStructure() const;
#endif

	const SC_FilePath& GetSourceFile() const;

private:
	bool InitDefault(const SGfx_MeshCreateParams& aCreateParams);
#if ENABLE_MESH_SHADERS
	bool InitForMeshShaders(const SGfx_MeshCreateParams& aCreateParams);
#endif

#if ENABLE_RAYTRACING
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
	SC_Ref<SR_BufferResource> mIndexBufferResource;
#if ENABLE_MESH_SHADERS
	SGfx_MeshletBuffers mMeshletBuffers;
#endif
#if ENABLE_RAYTRACING
	SC_Ref<SR_BufferResource> mAccelerationStructure;
#endif

	bool mIsMeshletMesh;
	bool mUsingMeshlets;
};

