
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
	assert(aData && (aSize > 0));

	if (mDataPtr)
	{
		SC_Memcpy(mDataPtr + aOffset, aData, aSize);
	}
	else
	{
		SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->CreateCommandList(SR_CommandListType::Copy);
		cmdList->Begin();
		cmdList->UpdateBuffer(this, aOffset, aData, (uint32)aSize);
		cmdList->End();

		SR_Fence fence = SR_RenderDevice::gInstance->GetCommandQueue(SR_CommandListType::Copy)->SubmitCommandList(cmdList.get(), "SR_BufferResource::UpdateData");
		SR_RenderDevice::gInstance->WaitForFence(fence);
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
