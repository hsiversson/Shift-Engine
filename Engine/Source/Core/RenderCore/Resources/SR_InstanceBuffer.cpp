#include "SR_InstanceBuffer.h"

SR_InstanceBuffer::SR_InstanceBuffer()
	: mCurrentAddress(0)
	, mOffsetStride(0)
	, mMappedDataPtr(nullptr)
	, mIsActive(false)
{

}

SR_InstanceBuffer::~SR_InstanceBuffer()
{

}

bool SR_InstanceBuffer::Init(uint32 aInitialCount, const char* aDebugName)
{
	SR_BufferResourceProperties props;
	props.mElementSize = sizeof(uint32);
	props.mElementCount = aInitialCount;
	props.mBindFlags = SR_BufferBindFlag_Buffer;
	props.mWritable = false;
	props.mDebugName = aDebugName;

	mBufferResource = SR_RenderDevice::gInstance->CreateBufferResource(props);

	mCurrentAddress = mBufferResource->GetGPUAddressStart();
	mOffsetStride = props.mElementSize;

	return true;
}

void SR_InstanceBuffer::Begin()
{
	// Check if any previous fences has finished, and if so free their regions
	// Begin a new buffer region
	mIsActive = true;
	mCurrentAddress = mBufferResource->GetGPUAddressStart();
}

void* SR_InstanceBuffer::Alloc(uint32 /*aNumBytes*/, uint32& /*aOutOffset*/)
{
	//SC_ASSERT(mIsActive, "Allocs must happen in between BeginFrame and EndFrame");
	//
	//uint32 lastValue = mWritePos.Push(aNumBytes);
	//if (lastValue + aNumBytes > mBufferSize)
	//{
	//	//SC_ERROR("Insufficient capacity in InstanceBuffer");
	//	mWritePos.Pop(aNumBytes);
	//	aOutOffset = 0;
	//	return nullptr;
	//}
	//
	//aOutOffset = lastValue / sizeof(SC_Vector4);
	//if ((lastValue + aNumBytes) > mMaxBytesUsed)
	//{
	//	mMaxBytesUsed = lastValue + aNumBytes;
	//	mNewMax = true;
	//}
	//
	//return ((uint8*)mMappedDataPtr) + lastValue;
	return nullptr;
}

void SR_InstanceBuffer::End()
{
	// Record frame span region and signal a fence
	mIsActive = false;
}

void SR_InstanceBuffer::Grow(uint32 /*aNewSize*/)
{
}
