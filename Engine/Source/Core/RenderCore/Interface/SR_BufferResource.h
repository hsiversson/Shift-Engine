#pragma once
#include "RenderCore/Resources/SR_Resource.h"

class SR_Heap;
struct SR_BufferResourceProperties
{
	SR_BufferResourceProperties() : mElementSize(0), mElementCount(0), mBindFlags(0), mHeap(nullptr), mInitialData(nullptr), mWritable(false), mIsUploadBuffer(false), mDebugName(nullptr) {}

	uint32 mElementSize;
	uint32 mElementCount;
	uint32 mBindFlags;

	SR_Heap* mHeap;

	const void* mInitialData;

	bool mWritable;
	bool mIsUploadBuffer;

	const char* mDebugName;
};

class SR_BufferResource : public SR_Resource
{
public:
	SR_BufferResource(const SR_BufferResourceProperties& aProperties);
	virtual ~SR_BufferResource();

	const SR_BufferResourceProperties& GetProperties() const;

	void UpdateData(uint32 aOffset, const void* aData, uint64 aSize);
	void* GetDataPtr() const;

	virtual uint64 GetGPUAddressStart() const;
protected:
	SR_BufferResourceProperties mProperties;
	uint8* mDataPtr;
private:
};

