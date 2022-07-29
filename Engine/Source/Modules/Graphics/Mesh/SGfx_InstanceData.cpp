#include "SGfx_InstanceData.h"

SGfx_InstanceData::SGfx_InstanceData()
	: mCurrentOffset(0)
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
	SC_MutexLock lock(mMutex);

	if (mData.IsEmpty())
		return;

	bool createBuffer = !mBuffer || (mBuffer->GetProperties().mElementCount < mData.GetByteSize());
	if (createBuffer)
	{
		SR_BufferResourceProperties resourceProps = {};
		resourceProps.mBindFlags = SR_BufferBindFlag_Buffer;
		resourceProps.mElementCount = mData.GetByteSize();
		resourceProps.mElementSize = 1;
		resourceProps.mDebugName = "InstanceData";
		mBufferResource = SR_RenderDevice::gInstance->CreateBufferResource(resourceProps, mData.GetBuffer());
	}
	else
		mBufferResource->UpdateData(0, mData.GetBuffer(), mData.GetByteSize());

	SR_BufferProperties bufferProps = {};
	bufferProps.mElementCount = mData.GetByteSize();
	bufferProps.mFirstElement = 0;
	bufferProps.mType = SR_BufferType::Bytes;
	mBuffer = SR_RenderDevice::gInstance->CreateBuffer(bufferProps, mBufferResource);
}

SR_Buffer* SGfx_InstanceData::GetBuffer() const
{
	return mBuffer;
}