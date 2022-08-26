#pragma once
#include "RenderCore/Interface/SR_TextureResource.h"

#if SR_ENABLE_VULKAN

class SR_TextureResource_Vk final : public SR_TextureResource
{
public:
	SR_TextureResource_Vk(const SR_TextureResourceProperties& aProperties);
	~SR_TextureResource_Vk();

private:
	VkImage mVkImage;
};

#endif