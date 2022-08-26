#include "SR_Buffer_Vk.h"

#if SR_ENABLE_VULKAN

SR_Buffer_Vk::SR_Buffer_Vk(const SR_BufferProperties& aProperties, const SC_Ref<SR_BufferResource>& aResource)
	: SR_Buffer(aProperties, aResource)
	, mVkBufferView(VK_NULL_HANDLE)
{

}

SR_Buffer_Vk::~SR_Buffer_Vk()
{

}

#endif
