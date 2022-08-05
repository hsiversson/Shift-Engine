#pragma once
#include "RenderCore/Defines/SR_RenderEnums.h"

#if SR_ENABLE_DX12
struct ID3D12Resource;
#endif

struct SR_GlobalResourceState
{
	SR_ResourceState mCurrentLocalState;
	SR_ResourceState mCurrentGlobalState;
	SR_ResourceState mPrevState;
};

class SR_Resource : public SC_ReferenceCounted
{
public:
	SR_Resource() {}
	virtual ~SR_Resource() {}

protected:
	bool CanDestruct(volatile uint32& aReferenceCount) override;
};

class SR_TrackedResource : public SR_Resource
{
public:
	SR_TrackedResource();
	virtual ~SR_TrackedResource();

	uint32 mLatestResourceState;

#if SR_ENABLE_DX12
	ID3D12Resource* GetD3D12Resource() const;
	ID3D12Resource* mTrackedD3D12Resource;
#endif
};

