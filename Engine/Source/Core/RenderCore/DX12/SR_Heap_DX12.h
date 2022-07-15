#pragma once
#include "RenderCore/Interface/SR_Heap.h"
#include "RenderCore/Resources/SR_RingBuffer.h"

#if SR_ENABLE_DX12

struct ID3D12Heap;

class SR_Heap_DX12 : public SR_Heap
{
public:
	SR_Heap_DX12(const SR_HeapProperties& aProperties);
	~SR_Heap_DX12();

	bool Init();

	void EndFrame();

	const uint64 GetOffset(uint64 aSize, uint64 aAlignment) override;

	ID3D12Heap* GetD3D12Heap() const;
	uint64 GetHeapOffset() const;
private:
	SR_ComPtr<ID3D12Heap> mD3D12Heap;
	SR_RingBuffer mRingBuffer;
	volatile uint64 mHeapOffset;
};

#endif //SR_ENABLE_DX12

