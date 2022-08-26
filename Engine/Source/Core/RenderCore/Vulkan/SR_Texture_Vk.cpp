#include "SR_Texture_Vk.h"

#if SR_ENABLE_VULKAN

SR_Texture_Vk::SR_Texture_Vk(const SR_TextureProperties& aProperties, const SC_Ref<SR_TextureResource>& aResource)
	: SR_Texture(aProperties, aResource)
	, mVkImageView(VK_NULL_HANDLE)
{

}

SR_Texture_Vk::~SR_Texture_Vk()
{

}

#endif
