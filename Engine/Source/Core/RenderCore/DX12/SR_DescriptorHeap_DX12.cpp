
#include "SR_DescriptorHeap_DX12.h"

#if ENABLE_DX12
#include "SR_RenderDevice_DX12.h"

SR_DescriptorHeap_DX12::SR_DescriptorHeap_DX12(uint32 aNumDescriptors, const SR_DescriptorHeapType& aType, bool aShaderVisible, const char* aDebugName)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = (aShaderVisible) ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NumDescriptors = aNumDescriptors;
	desc.NodeMask = 0;

	switch (aType)
	{
	case SR_DescriptorHeapType::CBV_SRV_UAV:
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		break;
	case SR_DescriptorHeapType::RTV:
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		break;
	case SR_DescriptorHeapType::DSV:
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		break;
	case SR_DescriptorHeapType::SAMPLER:
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		break;
	default:
		assert(false);
		return;
	}

	ID3D12Device* device = SR_RenderDevice_DX12::gD3D12Instance->GetD3D12Device();
	HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mD3D12DescriptorHeap));

	if (!VerifyHRESULT(hr))
	{
		assert(false);
		return;
	}

	mDescriptorTypeSize = device->GetDescriptorHandleIncrementSize(desc.Type);
	mIsShaderVisible = aShaderVisible;

	mFreeIndices.Reserve(aNumDescriptors);
	for (int i = aNumDescriptors - 1; i >= 0; --i)
	{
		mFreeIndices.Add(i);
	}

	if (aDebugName)
	{
		mD3D12DescriptorHeap->SetName(SC_UTF8ToUTF16(aDebugName).c_str());
	}
}

SR_DescriptorHeap_DX12::~SR_DescriptorHeap_DX12()
{

}

SR_Descriptor SR_DescriptorHeap_DX12::Alloc()
{
	const uint32 index = GetIndex();
	uint32 offset = index * mDescriptorTypeSize;

	SR_Descriptor allocation;
	allocation.mDescriptorHandleCPU = mD3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + offset;

	if (mIsShaderVisible)
		allocation.mDescriptorHandleGPU = mD3D12DescriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr + offset;

	allocation.mHeapIndex = index;

	return allocation;
}

void SR_DescriptorHeap_DX12::Free(const SR_Descriptor& aAllocation)
{
	ReturnIndex(aAllocation.mHeapIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE SR_DescriptorHeap_DX12::GetD3D12HeapStartHandleCPU() const
{
	return mD3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_GPU_DESCRIPTOR_HANDLE SR_DescriptorHeap_DX12::GetD3D12HeapStartHandleGPU() const
{
	return mD3D12DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
}

ID3D12DescriptorHeap* SR_DescriptorHeap_DX12::GetD3D12DescriptorHeap() const
{
	return mD3D12DescriptorHeap.Get();
}

uint32 SR_DescriptorHeap_DX12::GetIndex()
{
	uint32 descriptorIndex = 0;
	{
		SC_MutexLock lock(mInternalMutex);

		assert((mFreeIndices.Count() > 0) && "Descriptor heap is full.");
		descriptorIndex = mFreeIndices.Last();
		mFreeIndices.RemoveLast();
	}
	return descriptorIndex;
}

void SR_DescriptorHeap_DX12::ReturnIndex(uint32 aIndex)
{
	SC_MutexLock lock(mInternalMutex);
	mFreeIndices.Add(uint32(aIndex));
}

#endif
