#include "SR_TextureResource_Vk.h"

#if SR_ENABLE_VULKAN


SR_TextureResource_Vk::SR_TextureResource_Vk(const SR_TextureResourceProperties& aProperties)
	: SR_TextureResource(aProperties)
	, mVkImage(VK_NULL_HANDLE)
{

}

SR_TextureResource_Vk::~SR_TextureResource_Vk()
{

}

#endif