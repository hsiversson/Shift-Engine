#pragma once
#include "RenderCore/Defines/SR_Format.h"
#include "SR_TextureResource.h"
#include "SR_Descriptor.h"

enum class SR_TextureDimension
{
	Texture1D,
	Texture2D,
	Texture3D,
	TextureCube
};

struct SR_TextureProperties : public SR_TextureRange
{
	SR_TextureProperties() : mFormat(SR_Format::UNKNOWN), mDimension(SR_TextureDimension::Texture2D), mWritable(false) {}
	SR_TextureProperties(const SR_Format& aFormat) : mFormat(aFormat), mDimension(SR_TextureDimension::Texture2D), mWritable(false) {}

	SR_Format mFormat;
	SR_TextureDimension mDimension;

	bool mWritable;
};

class SR_Texture
{
public:
	const SR_TextureProperties& GetProperties() const;
	SR_TextureResource* GetResource() const;
	const SR_TextureResourceProperties& GetResourceProperties() const;

	const SR_Descriptor& GetDescriptor() const;
	uint32 GetDescriptorHeapIndex() const;

	bool IsSRV() const;
	bool IsUAV() const;

protected:
	SR_Texture(const SR_TextureProperties& aProperties, const SC_Ref<SR_TextureResource>& aResource);
	virtual ~SR_Texture();

	SR_TextureProperties mProperties;
	SC_Ref<SR_TextureResource> mResource;
	SR_Descriptor mDescriptor;
};

