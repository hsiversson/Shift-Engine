#pragma once
#include "RenderCore/Interface/SR_Buffer.h"

#if SR_ENABLE_VULKAN

class SR_Buffer_Vk final : public SR_Buffer
{
public:
	SR_Buffer_Vk(const SR_BufferProperties& aProperties, const SC_Ref<SR_BufferResource>& aResource);
	~SR_Buffer_Vk();

private:
	VkBufferView mVkBufferView;
};

#endif