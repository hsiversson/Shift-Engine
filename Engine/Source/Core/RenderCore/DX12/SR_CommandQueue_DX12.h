#pragma once

#if ENABLE_DX12
#include "RenderCore/Interface/SR_CommandQueue.h"
#include "SR_CommandList_DX12.h"

struct ID3D12CommandQueue;
struct ID3D12Fence;

class SR_RenderDevice_DX12;
class SR_FenceResource_DX12;

class SR_CommandQueue_DX12 : public SR_CommandQueue
{
public:
	SR_CommandQueue_DX12(SR_RenderDevice_DX12* aDevice);
	~SR_CommandQueue_DX12();

	bool Init(const SR_CommandListType& aType, const char* aDebugName = nullptr);

	void SubmitCommandLists(SR_CommandList** aCommandLists, uint32 aNumCommandLists, const char* aEventName = nullptr) override;
	void InsertFence(const SR_Fence& aFence) override;
	void InsertWait(const SR_Fence& aFence) override;

	void BeginEvent(const char* aName) override;
	void EndEvent() override;

	ID3D12CommandQueue* GetD3D12CommandQueue() const;

private:
	SR_ComPtr<ID3D12CommandQueue> mD3D12CommandQueue;
	SC_Ref<SR_FenceResource_DX12> mFence;

	SR_RenderDevice_DX12* mRenderDevice;
};

#endif