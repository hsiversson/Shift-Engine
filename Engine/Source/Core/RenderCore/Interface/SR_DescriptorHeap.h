#pragma once
#include "SR_Descriptor.h"

enum class SR_DescriptorHeapType
{
	CBV_SRV_UAV,
	RTV,
	DSV,
	SAMPLER
};

class SR_DescriptorHeap
{
public:
	virtual ~SR_DescriptorHeap() {}

	virtual SR_Descriptor Alloc() = 0;
	virtual void Free(const SR_Descriptor& aAllocation) = 0;
};

