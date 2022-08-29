#pragma once
#include "RenderCore/Defines/SR_RenderEnums.h"
#include "RenderCore/Defines/SR_Format.h"

#if SR_ENABLE_RAYTRACING

class SR_BufferResource;
struct SR_RaytracingGeometryData
{
	SR_RaytracingGeometryData() : mVertexFormat(SR_Format::UNKNOWN), mVertexBuffer(nullptr), mIndexBuffer(nullptr) {}

	SR_Format mVertexFormat;
	SR_BufferResource* mVertexBuffer;
	SR_BufferResource* mIndexBuffer;
};

struct SR_RaytracingInstanceProperties
{
	SR_RaytracingInstanceProperties() 
		: mTransform(SC_Matrix::Identity()) 
		, mAccelerationStructureGPUAddress(0)
		, mInstanceId(0)
		, mInstanceMask(0)
		, mHitGroup(0)
		, mFaceCullingMode(SR_CullMode::Back)
		, mIsOpaque(true)
	{}

	SC_Matrix mTransform;
	uint64 mAccelerationStructureGPUAddress;
	uint32 mInstanceId;
	uint8 mInstanceMask;
	uint8 mHitGroup;

	SR_CullMode mFaceCullingMode;
	bool mIsOpaque;
};

struct SR_AccelerationStructureInputs
{
	SR_AccelerationStructureInputs() : mFlags(0), mIsTopLevel(false), mGeometryData(nullptr) {}
	~SR_AccelerationStructureInputs() {}

	enum BuildFlags
	{
		BuildFlag_AllowUpdate = 0x1,
		BuildFlag_OptimizeTraceSpeed = 0x2,
		BuildFlag_OptimizeBuildSpeed = 0x4,
		BuildFlag_OptimizeMemoryFootprint = 0x8,
		BuildFlag_Update = 0x10
	};

	uint32 mFlags;
	bool mIsTopLevel;
	union
	{
		const SC_Array<SR_RaytracingGeometryData>* mGeometryData;
		const SC_Array<SR_RaytracingInstanceProperties>* mInstanceProperties;
	};
};

struct alignas(16) SR_RaytracingInstanceData
{
	SC_Matrix mTransform;

	uint32 mVertexBufferDescriptorIndex;
	uint32 mVertexStride;
	uint32 mIndexBufferDescriptorIndex;
	uint32 mMaterialIndex;

	uint32 mVertexPositionByteOffset;
	uint32 mVertexNormalByteOffset;
	uint32 mVertexTangentByteOffset;
	uint32 mVertexBitangentByteOffset;

	uint32 mVertexUVByteOffset;
	uint32 __pad[3];
};
#endif