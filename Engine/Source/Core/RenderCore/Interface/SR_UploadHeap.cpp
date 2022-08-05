#include "SR_UploadHeap.h"
#include "SR_Heap.h"


SR_UploadHeap::SR_UploadHeap()
{

}

SR_UploadHeap::~SR_UploadHeap()
{

}

bool SR_UploadHeap::Init()
{
	SR_HeapProperties heapProps = {};
	heapProps.mByteSize = MB(48);
	heapProps.mCreateResourcesUninitialized = true;
	heapProps.mResourceType = SR_HeapResourceType::Buffer;
	heapProps.mType = SR_HeapType::Upload;
	mHeap = SR_RenderDevice::gInstance->CreateHeap(heapProps);

	return true;
}

void SR_UploadHeap::CopyTexture(SR_TextureResource* aTargetResource, const void* aData, uint64 aSize, uint64 aOffset)
{
	auto task = [aTargetResource, aData, aSize, aOffset]()
	{
		//SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
		//cmdList->CopyTexture()
	};
	SC_Ref<SR_TaskEvent> taskEvent = SR_RenderDevice::gInstance->PostCopyTask(task);
}

void SR_UploadHeap::CopyBuffer(SR_BufferResource* /*aTargetResource*/, const void* /*aData*/, uint64 /*aSize*/, uint64 /*aOffset*/)
{
}
