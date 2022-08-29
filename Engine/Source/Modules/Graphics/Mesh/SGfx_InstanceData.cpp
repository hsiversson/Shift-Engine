#include "SGfx_InstanceData.h"

SGfx_InstanceData::SGfx_InstanceData(const char* aDebugName)
	: mCurrentOffset(0)
	, mDebugName(aDebugName ? aDebugName : "SGfx_InstanceData")
{

}

SGfx_InstanceData::~SGfx_InstanceData()
{

}

void SGfx_InstanceData::Add(uint32& aOffsetOut, uint32 aCount, const SC_Vector4* aData)
{
	SC_MutexLock lock(mMutex);
	mData.Add(aData, aCount);

	aOffsetOut = mCurrentOffset;
	mCurrentOffset += aCount * sizeof(SC_Vector4);
}

void SGfx_InstanceData::Clear()
{
	SC_MutexLock lock(mMutex);
	mData.RemoveAll(); 
	mCurrentOffset = 0;
}

void SGfx_InstanceData::Prepare()
{
}

SR_Buffer* SGfx_InstanceData::GetBuffer()
{
	SC_MutexLock lock(mMutex);

	static constexpr uint32 gZeroData = 0;
	const uint32 byteSize = SC_Max(mData.GetByteSize(), sizeof(uint32));
	uint64 offset = 0;
	SR_BufferResource* buf = SR_RenderDevice::gInstance->GetTempBufferResource(offset, SR_BufferBindFlag_Buffer, byteSize, (mData.IsEmpty()) ? (const void*)&gZeroData : (const void*)mData.GetBuffer(), 1);

	SR_BufferProperties bufferProps = {};
	bufferProps.mElementCount = byteSize;
	bufferProps.mOffset = (uint32)offset;
	bufferProps.mType = SR_BufferType::Bytes;
	mBuffer = SR_RenderDevice::gInstance->CreateBuffer(bufferProps, buf);

	return mBuffer;
}