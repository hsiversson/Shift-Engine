#pragma once
#if SR_ENABLE_DX12
#include "Platform/Async/SC_Mutex.h"
#include "RenderCore/Interface/SR_DescriptorHeap.h"

struct ID3D12DescriptorHeap;

class SR_DescriptorHeap_DX12 : public SR_DescriptorHeap
{
public:
	SR_DescriptorHeap_DX12(uint32 aNumDescriptors, const SR_DescriptorHeapType& aType, bool aShaderVisible = false, const char* aDebugName = nullptr);
	~SR_DescriptorHeap_DX12();

	SR_Descriptor Alloc() override;
	void Free(const SR_Descriptor& aAllocation) override;

	D3D12_CPU_DESCRIPTOR_HANDLE GetD3D12HeapStartHandleCPU() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetD3D12HeapStartHandleGPU() const;
	ID3D12DescriptorHeap* GetD3D12DescriptorHeap() const;

private:
	uint32 GetIndex();
	void ReturnIndex(uint32 aIndex);

	SC_Array<uint32> mFreeIndices;
	SR_ComPtr<ID3D12DescriptorHeap> mD3D12DescriptorHeap;
	SC_Mutex mInternalMutex;
	uint32 mDescriptorTypeSize;
	uint32 mMaxNumDescriptors;
	bool mIsShaderVisible;
};
#endif

