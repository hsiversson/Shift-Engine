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
	SR_RenderTaskManager* taskManager = SR_RenderDevice::gInstance->GetRenderTaskManager();

	auto task = [aTargetResource, aData, aSize, aOffset]()
	{
		//SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
		//cmdList->CopyTexture()
	};

	taskManager->SubmitTask(task, SR_CommandListType::Copy, nullptr);
}

void SR_UploadHeap::CopyBuffer(SR_BufferResource* /*aTargetResource*/, const void* /*aData*/, uint64 /*aSize*/, uint64 /*aOffset*/)
{
}
