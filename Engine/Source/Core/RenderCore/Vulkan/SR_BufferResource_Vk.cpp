#include "SR_BufferResource_Vk.h"

#if SR_ENABLE_VULKAN

SR_BufferResource_Vk::SR_BufferResource_Vk(const SR_BufferResourceProperties& aProperties)
	: SR_BufferResource(aProperties)
	, mVkBuffer(VK_NULL_HANDLE)
{

}

SR_BufferResource_Vk::~SR_BufferResource_Vk()
{

}

VkBuffer SR_BufferResource_Vk::GetVkBuffer() const
{
	return mVkBuffer;
}

#endif
