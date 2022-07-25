#include "SR_CommandQueue_DX12.h"

#if SR_ENABLE_DX12
#include "SR_RenderDevice_DX12.h"
#include "SR_Fence_DX12.h"

SR_CommandQueue_DX12::SR_CommandQueue_DX12(SR_RenderDevice_DX12* aDevice)
	: mRenderDevice(aDevice)
{

}

SR_CommandQueue_DX12::~SR_CommandQueue_DX12()
{

}

bool SR_CommandQueue_DX12::Init(const SR_CommandListType& aType, const char* aDebugName)
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.NodeMask = 0;
	
	switch (aType)
	{
		case SR_CommandListType::Copy:
			desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
			break;
		case SR_CommandListType::Compute:
			desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			break;
		case SR_CommandListType::Graphics:
		default:
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			break;
	}

	HRESULT hr = mRenderDevice->GetD3D12Device()->CreateCommandQueue(&desc, IID_PPV_ARGS(&mD3D12CommandQueue));
	if (!VerifyHRESULT(hr))
	{
		SC_ASSERT(false, "Could not create command queue.");
		return false;
	}

	mFence = SC_MakeRef<SR_FenceResource_DX12>();
	if (!mFence->Init())
		return false;

	if (aDebugName)
	{
		std::wstring debugName = SC_UTF8ToUTF16(aDebugName);
		mD3D12CommandQueue->SetName(debugName.c_str());
	}

	mType = aType;
	return true;
}

void SR_CommandQueue_DX12::SubmitCommandLists(SR_CommandList** aCommandLists, uint32 aNumCommandLists, const char* aEventName)
{
	//SC_PROFILER_FUNCTION();
	if (aEventName)
		BeginEvent(aEventName);

	SC_Array<ID3D12CommandList*> d3d12CmdLists;
	d3d12CmdLists.Reserve(aNumCommandLists);
	for (uint32 i = 0; i < aNumCommandLists; ++i)
	{
		SR_CommandList_DX12* dx12cmdList = static_cast<SR_CommandList_DX12*>(aCommandLists[i]);
		d3d12CmdLists.Add(dx12cmdList->GetD3D12CommandList());

		const SC_Array<SR_Fence>& fenceWaits = dx12cmdList->GetFenceWaits();
		for (const SR_Fence& fence : fenceWaits)
			InsertWait(fence);
	}

	mD3D12CommandQueue->ExecuteCommandLists(d3d12CmdLists.Count(), d3d12CmdLists.GetBuffer());

	if (aEventName)
		EndEvent();
}

SR_Fence SR_CommandQueue_DX12::GetNextFence()
{
	return SR_Fence(mFence->GetNextValue(), mType);
}

void SR_CommandQueue_DX12::SignalFence(const SR_Fence& aFence)
{
	mD3D12CommandQueue->Signal(mFence->GetD3D12Fence(), aFence.mValue);
}

void SR_CommandQueue_DX12::InsertWait(const SR_Fence& aFence)
{
	mD3D12CommandQueue->Wait(mFence->GetD3D12Fence(), aFence.mValue);
}

bool SR_CommandQueue_DX12::IsFencePending(const SR_Fence& aFence)
{
	return mFence->IsPending(aFence.mValue);
}

bool SR_CommandQueue_DX12::WaitForFence(const SR_Fence& aFence, bool aBlock)
{
	return mFence->Wait(aFence.mValue, aBlock);
}

void SR_CommandQueue_DX12::BeginEvent(const char* aName)
{
#if SR_ENABLE_PIX
	//SC_PROFILER_FUNCTION();
	PIXBeginEvent(mD3D12CommandQueue.Get(), 0, "%s", aName);
#else
	(void)aName;
#endif
}

void SR_CommandQueue_DX12::EndEvent()
{
#if SR_ENABLE_PIX
	//SC_PROFILER_FUNCTION();
	PIXEndEvent(mD3D12CommandQueue.Get());
#endif
}

ID3D12CommandQueue* SR_CommandQueue_DX12::GetD3D12CommandQueue() const
{
	return mD3D12CommandQueue.Get();
}

#endif
