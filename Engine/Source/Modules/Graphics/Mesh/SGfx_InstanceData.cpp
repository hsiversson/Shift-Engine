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

SR_TempBuffer SGfx_InstanceData::GetBuffer() const
{
	SC_MutexLock lock(mMutex);

	if (mData.IsEmpty())
		return SR_TempBuffer();

	SR_BufferResourceProperties bufferProps;
	bufferProps.mBindFlags = SR_BufferBindFlag_Buffer;
	bufferProps.mElementCount = mData.GetByteSize();
	bufferProps.mElementSize = 1;
	bufferProps.mInitialData = mData.GetBuffer();
	bufferProps.mDebugName = "InstanceData";

	return SR_RenderDevice::gInstance->CreateTempBuffer(bufferProps);
}