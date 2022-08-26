#pragma once
#include "RenderCore/Interface/SR_BufferResource.h"

#if SR_ENABLE_VULKAN

class SR_BufferResource_Vk final : public SR_BufferResource
{
public:
	SR_BufferResource_Vk(const SR_BufferResourceProperties& aProperties);
	~SR_BufferResource_Vk();

	VkBuffer GetVkBuffer() const;

private:
	VkBuffer mVkBuffer;
};

#endif