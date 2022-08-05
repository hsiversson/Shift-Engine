#pragma once
#include "RenderCore/Interface/SR_BufferResource.h"

#if SR_ENABLE_DX12

struct ID3D12Resource;

class SR_BufferResource_DX12 : public SR_BufferResource
{
public:
	SR_BufferResource_DX12(const SR_BufferResourceProperties& aProperties);
	~SR_BufferResource_DX12();

	bool Init(const void* aInitialData);

	uint64 GetGPUAddressStart() const override;
};

#endif

