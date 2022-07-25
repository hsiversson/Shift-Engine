#include "SR_BufferResource_DX12.h"

#if SR_ENABLE_DX12
#include "SR_RenderDevice_DX12.h"
#include "SR_Heap_DX12.h"

SR_BufferResource_DX12::SR_BufferResource_DX12(const SR_BufferResourceProperties& aProperties)
	: SR_BufferResource(aProperties)
	, mD3D12Resource(nullptr)
{

}

SR_BufferResource_DX12::~SR_BufferResource_DX12()
{
	if (mD3D12Resource)
	{
		mD3D12Resource->Release();
		mD3D12Resource = nullptr;
	}
}

bool SR_BufferResource_DX12::Init(const void* aInitialData)
{
	D3D12_RESOURCE_DESC resourceDesc = SR_GetD3D12ResourceDesc(mProperties);

	if (mProperties.mWritable)
		resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;

	if (mProperties.mBindFlags & SR_BufferBindFlag_ConstantBuffer)
	{
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
		resourceDesc.Width = SC_Align(resourceDesc.Width, 16);
	}

	if (mProperties.mBindFlags & SR_BufferBindFlag_Readback)
	{
		heapProps.Type = D3D12_HEAP_TYPE_READBACK;
		initialState = D3D12_RESOURCE_STATE_COPY_DEST;
	}

	if (mProperties.mBindFlags & SR_BufferBindFlag_RaytracingBuffer)
		initialState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;

	//if (mProperties.mBindFlags == SR_BufferBindFlag_Buffer && mProperties.mWritable)
	//	initialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	//
	//if (aInitialData)
	//	initialState = D3D12_RESOURCE_STATE_COPY_DEST;

	if (mProperties.mIsUploadBuffer)
	{
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
	}

	HRESULT hr = S_OK;
	if (mProperties.mHeap)
	{
		static constexpr uint32 alignment = 65536;
		const D3D12_RESOURCE_ALLOCATION_INFO allocInfo = SR_RenderDevice_DX12::gInstance->GetD3D12Device()->GetResourceAllocationInfo(0, 1, &resourceDesc);

		SR_Heap_DX12* heap = static_cast<SR_Heap_DX12*>(mProperties.mHeap);

		hr = SR_RenderDevice_DX12::gInstance->GetD3D12Device()->CreatePlacedResource(
			heap->GetD3D12Heap(),
			heap->GetOffset(allocInfo.SizeInBytes, allocInfo.Alignment),

			&resourceDesc,
			initialState,
			nullptr,
			IID_PPV_ARGS(&mD3D12Resource)
		);
	}
	else
		hr = SR_RenderDevice_DX12::gInstance->GetD3D12Device()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, initialState, nullptr, IID_PPV_ARGS(&mD3D12Resource)); // CRASH nvwgf2umx.dll - temp resources

	if (!VerifyHRESULT(hr))
	{
		SC_ERROR("Could not create buffer with id: {} \n", mProperties.mDebugName);
		return false;
	}
	mTrackedD3D12Resource = mD3D12Resource;

	if (heapProps.Type == D3D12_HEAP_TYPE_UPLOAD)
	{
		D3D12_RANGE readRange{ 0, 0 };
		void* dataPtr = nullptr;
		hr = mD3D12Resource->Map(0, &readRange, &dataPtr);
		if (!VerifyHRESULT(hr))
			SC_ASSERT(false, "Could not map resource.");

		mDataPtr = (uint8*)dataPtr;
	}

	if (aInitialData)
	{
		SR_BufferResourceProperties uploadProps(mProperties);
		uploadProps.mIsUploadBuffer = true;
		uploadProps.mHeap = nullptr;

		SR_BufferResource_DX12* uploadBuffer = new SR_BufferResource_DX12(uploadProps);
		if (!uploadBuffer->Init(nullptr))
		{
			SC_ASSERT(false, "Could not init upload buffer.");
		}

		uploadBuffer->UpdateData(0, aInitialData, resourceDesc.Width);

		auto UploadData = [this, uploadBuffer]()
		{
			SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
			cmdList->CopyResource(this, uploadBuffer);
		};

		SC_Ref<SR_TaskEvent> taskEvent = SC_MakeRef<SR_TaskEvent>();
		SR_RenderDevice::gInstance->GetQueueManager()->SubmitTask(UploadData, SR_CommandListType::Copy, taskEvent);

		taskEvent->mCPUEvent.Wait();
		taskEvent->mFence.Wait();

		delete uploadBuffer;
	}

	if (mProperties.mDebugName)
	{
		mD3D12Resource->SetName(SC_UTF8ToUTF16(mProperties.mDebugName).c_str());
	}

	return true;
}

uint64 SR_BufferResource_DX12::GetGPUAddressStart() const
{
	return mD3D12Resource->GetGPUVirtualAddress();
}

ID3D12Resource* SR_BufferResource_DX12::GetD3D12Resource() const
{
	return mD3D12Resource;
}

#endif