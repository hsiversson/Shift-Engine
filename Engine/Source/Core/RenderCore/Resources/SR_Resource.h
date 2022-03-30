#pragma once
#include "RenderCore/Defines/SR_RenderEnums.h"

#if ENABLE_DX12
struct ID3D12Resource;
#endif

struct SR_GlobalResourceState
{
	SR_ResourceState mCurrentLocalState;
	SR_ResourceState mCurrentGlobalState;
	SR_ResourceState mPrevState;
};

class SR_Resource
{
public:
	SR_Resource();
	virtual ~SR_Resource();

	uint32 mLatestResourceState;

#if ENABLE_DX12
	ID3D12Resource* mTrackedD3D12Resource;
#endif
};

