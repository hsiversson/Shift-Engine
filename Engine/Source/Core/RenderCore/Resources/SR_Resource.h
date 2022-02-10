#pragma once
#include "RenderCore/Defines/SR_RenderEnums.h"

#if ENABLE_DX12
struct ID3D12Resource;
#endif

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

