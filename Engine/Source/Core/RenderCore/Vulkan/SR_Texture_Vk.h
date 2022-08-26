#pragma once
#include "RenderCore/Interface/SR_Texture.h"

#if SR_ENABLE_VULKAN

class SR_Texture_Vk final : public SR_Texture
{
public:
	SR_Texture_Vk(const SR_TextureProperties& aProperties, const SC_Ref<SR_TextureResource>& aResource);
	~SR_Texture_Vk();

private:
	VkImageView mVkImageView;
};

#endif