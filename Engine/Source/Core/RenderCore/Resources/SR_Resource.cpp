#include "SR_Resource.h"

SR_TrackedResource::SR_TrackedResource()
	: mLatestResourceState(0)
#if SR_ENABLE_DX12
	, mTrackedD3D12Resource(nullptr)
#endif
{

}

SR_TrackedResource::~SR_TrackedResource()
{
#if SR_ENABLE_DX12
	if (mTrackedD3D12Resource)
		mTrackedD3D12Resource->Release();
#endif
}

#if SR_ENABLE_DX12
ID3D12Resource* SR_TrackedResource::GetD3D12Resource() const
{
	return mTrackedD3D12Resource;
}
#endif

bool SR_Resource::CanDestruct(volatile uint32& aReferenceCount)
{
	if (SC_ReferenceCounted::CanDestruct(aReferenceCount))
		return !SR_RenderDevice::gInstance->GetDelayDestructor()->AddToQueue(this);

	return false;
}
