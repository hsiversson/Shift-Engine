#pragma once
#include "SR_Descriptor.h"
#include "SR_TextureResource.h"

struct SR_RenderTargetProperties : public SR_TextureLevel
{
	SR_RenderTargetProperties() : mFormat(SR_Format::UNKNOWN) {}
	SR_RenderTargetProperties(const SR_Format& aFormat) : mFormat(aFormat) {}
	SR_Format mFormat;
};

class SR_RenderTarget
{
public:
	virtual ~SR_RenderTarget();
	const SR_RenderTargetProperties& GetProperties() const;

	SR_TextureResource* GetResource() const;

protected:
	SR_RenderTarget(const SR_RenderTargetProperties& aProperties, SC_Ref<SR_TextureResource> aResource);

	virtual void ReleaseDescriptor() {}

	SR_RenderTargetProperties mProperties;
	SC_Ref<SR_TextureResource> mResource;
	SR_Descriptor mDescriptor;
};

