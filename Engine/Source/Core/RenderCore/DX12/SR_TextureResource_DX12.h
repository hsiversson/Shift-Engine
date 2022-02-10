#pragma once

#if ENABLE_DX12
#include "RenderCore/Interface/SR_TextureResource.h"

struct ID3D12Resource;

class SR_Heap_DX12;

class SR_TextureResource_DX12 : public SR_TextureResource
{
public:
	SR_TextureResource_DX12(const SR_TextureResourceProperties& aProperties);
	SR_TextureResource_DX12(const SR_TextureResourceProperties& aProperties, ID3D12Resource* aResource);
	~SR_TextureResource_DX12();

	bool Init(const SR_PixelData* aInitialData, uint32 aDataCount);

	D3D12_TEXTURE_COPY_LOCATION GetCopyLocation(uint32 aSubresourceIndex) const;
	ID3D12Resource* GetD3D12Resource() const;
private:
	ID3D12Resource* mD3D12Resource;
};

void SR_MemcpyPixelData(void* aDest, const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& aDstFootprint, const SR_PixelData& aData, const SR_Format& aFormat);
uint32 SR_GetD3D12SubresourceIndex(const SR_TextureResourceProperties& aProperties, const SR_TextureLevel& aLevel);
#endif

