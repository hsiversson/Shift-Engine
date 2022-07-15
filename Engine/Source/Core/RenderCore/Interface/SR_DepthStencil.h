#pragma once
#include "SR_Descriptor.h"
#include "SR_TextureResource.h"

struct SR_DepthStencilProperties : public SR_TextureRange
{
	SR_DepthStencilProperties(const SR_Format& aFormat) : mFormat(aFormat) {}
	SR_Format mFormat;
};

class SR_DepthStencil
{
public:
	virtual ~SR_DepthStencil();

	const SR_DepthStencilProperties& GetProperties() const;
	SR_TextureResource* GetResource() const;

protected:
	SR_DepthStencil(const SR_DepthStencilProperties& aProperties, SC_Ref<SR_TextureResource> aResource);

	virtual void ReleaseDescriptor() {}

	SR_DepthStencilProperties mProperties;
	SC_Ref<SR_TextureResource> mResource;
	SR_Descriptor mDescriptor;
};

