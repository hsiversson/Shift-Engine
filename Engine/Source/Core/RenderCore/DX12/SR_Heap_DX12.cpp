#include "SR_Heap_DX12.h"

#if ENABLE_DX12
#include "SR_RenderDevice_DX12.h"

SR_Heap_DX12::SR_Heap_DX12(const SR_HeapProperties& aProperties)
	: SR_Heap(aProperties)
{

}

SR_Heap_DX12::~SR_Heap_DX12()
{

}

bool SR_Heap_DX12::Init()
{
	D3D12_HEAP_DESC heapDesc = {};
	heapDesc.SizeInBytes = mProperties.mByteSize;
	heapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	heapDesc.Flags = D3D12_HEAP_FLAG_NONE;

	switch (mProperties.mType)
	{
	case SR_HeapType::Default:
		heapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		break;
	case SR_HeapType::Upload:
		heapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
		break;
	case SR_HeapType::Readback:
		heapDesc.Properties.Type = D3D12_HEAP_TYPE_READBACK;
	default:
		SC_ASSERT(false, "Invalid heap type.");
		return false;
	}

	switch (mProperties.mResourceType)
	{
	case SR_HeapResourceType::Buffer:
		heapDesc.Flags |= D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
		break;
	case SR_HeapResourceType::Texture:
		heapDesc.Flags |= D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
		break;
	case SR_HeapResourceType::RenderTarget:
		heapDesc.Flags |= D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
		break;
	default:
		SC_ASSERT(false, "Invalid heap resource type.");
		return false;
	}

	if (mProperties.mCreateResourcesUninitialized)
		heapDesc.Flags |= D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;

	HRESULT hr = SR_RenderDevice_DX12::gD3D12Instance->GetD3D12Device()->CreateHeap(&heapDesc, IID_PPV_ARGS(&mD3D12Heap));
	if (!VerifyHRESULT(hr))
	{
		SC_ASSERT(false, "Could not create heap.");
		return false;
	}

	return true;
}

const uint64 SR_Heap_DX12::GetOffset(uint64 aSize, uint64 aAlignment)
{
	uint64 aligned = SC_Align(aSize, aAlignment);
	uint64 offset = mHeapOffset;
	mHeapOffset += aligned;
	return offset;
}

ID3D12Heap* SR_Heap_DX12::GetD3D12Heap() const
{
	return mD3D12Heap.Get();
}

uint64 SR_Heap_DX12::GetHeapOffset() const
{
	return mHeapOffset;
}

#endif //ENABLE_DX12