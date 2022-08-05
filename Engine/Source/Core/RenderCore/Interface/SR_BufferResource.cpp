
#include "SR_BufferResource.h"

SR_BufferResource::SR_BufferResource(const SR_BufferResourceProperties& aProperties)
	: mProperties(aProperties)
	, mDataPtr(nullptr)
{

}

SR_BufferResource::~SR_BufferResource()
{

}

const SR_BufferResourceProperties& SR_BufferResource::GetProperties() const
{
	return mProperties;
}

void SR_BufferResource::UpdateData(uint32 aOffset, const void* aData, uint64 aSize)
{
	SC_ASSERT(aData && (aSize > 0));

	if (mDataPtr)
	{
		SC_Memcpy(mDataPtr + aOffset, aData, aSize);
	}
	else
	{
		auto UploadData = [this, aOffset, aData, aSize]()
		{
			SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
			cmdList->UpdateBuffer(this, aOffset, aData, (uint32)aSize);
		};

		SC_Ref<SR_TaskEvent> taskEvent = SR_RenderDevice::gInstance->PostCopyTask(UploadData);
		taskEvent->mCPUEvent.Wait();
		taskEvent->mFence.Wait();
	}
}

void* SR_BufferResource::GetDataPtr() const
{
	return mDataPtr;
}

uint64 SR_BufferResource::GetGPUAddressStart() const
{
	return 0;
}
