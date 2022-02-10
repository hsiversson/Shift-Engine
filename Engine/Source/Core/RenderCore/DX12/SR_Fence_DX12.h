#pragma once
#include "RenderCore/Interface/SR_Fence.h"

#if ENABLE_DX12

struct ID3D12Fence;
class SR_FenceResource_DX12 : public SR_FenceResource
{
	friend class SR_CommandQueue_DX12;
public:
	SR_FenceResource_DX12();
	~SR_FenceResource_DX12();

	bool IsPending(uint64 aValue) override;
	bool Wait(uint64 aValue, bool aBlock = true) override;

	ID3D12Fence* GetD3D12Fence() const;

private:
	bool Init(const SR_CommandListType& aType);

	SR_ComPtr<ID3D12Fence> mD3D12Fence;
	volatile uint64 mFenceValue;
	volatile uint64 mLastKnownCompletedValue;

	SR_CommandQueue_DX12* mCommandQueue;
};

#endif

