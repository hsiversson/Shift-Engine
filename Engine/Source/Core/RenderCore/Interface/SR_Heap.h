#pragma once
#include "RenderCore/Defines/SR_RenderEnums.h"

enum class SR_HeapResourceType
{
	Buffer,
	Texture,
	RenderTarget,
	//_Debug,
	COUNT
};

enum class SR_HeapType
{
	Default,
	Upload,
	Readback,
	COUNT
};

struct SR_HeapProperties
{
	SR_HeapProperties() : mByteSize(0), mResourceType(SR_HeapResourceType::COUNT), mType(SR_HeapType::COUNT), mCmdQueueType(SR_CommandListType::Unknown), mCreateResourcesUninitialized(false), mDebugName(nullptr), mDebugPrint(false) {}

	uint64 mByteSize;
	SR_HeapResourceType mResourceType;
	SR_HeapType mType;
	SR_CommandListType mCmdQueueType;
	bool mCreateResourcesUninitialized;

	const char* mDebugName;
	bool mDebugPrint;
};

class SR_Heap
{
public:
	SR_Heap(const SR_HeapProperties& aProperties);
	virtual ~SR_Heap();

	virtual const uint64 GetOffset(uint64 aSize, uint64 aAlignment) = 0;

	const SR_HeapProperties& GetProperties() const;

protected:
	SR_HeapProperties mProperties;
};

