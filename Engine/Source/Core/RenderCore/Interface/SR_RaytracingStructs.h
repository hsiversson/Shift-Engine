#pragma once
#include "RenderCore/Defines/SR_RenderEnums.h"
#include "RenderCore/Defines/SR_Format.h"

#if ENABLE_RAYTRACING

class SR_BufferResource;
struct SR_RaytracingGeometryData
{
	SR_RaytracingGeometryData() : mVertexFormat(SR_Format::UNKNOWN), mVertexBuffer(nullptr), mIndexBuffer(nullptr) {}

	SR_Format mVertexFormat;
	SR_BufferResource* mVertexBuffer;
	SR_BufferResource* mIndexBuffer;
};

struct SR_RaytracingInstanceData
{
	SR_RaytracingInstanceData() 
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
		const SC_Array<SR_RaytracingInstanceData>* mInstanceData;
	};
};
#endif