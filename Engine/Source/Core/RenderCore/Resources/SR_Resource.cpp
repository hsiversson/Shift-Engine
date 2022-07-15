
#include "SR_Resource.h"

SR_Resource::SR_Resource()
	: mLatestResourceState(0)
#if SR_ENABLE_DX12
	, mTrackedD3D12Resource(nullptr)
#endif
{

}

SR_Resource::~SR_Resource()
{

}
