#include "SR_ShaderState_Vk.h"

#if SR_ENABLE_VULKAN

SR_ShaderState_Vk::SR_ShaderState_Vk()
{

}

SR_ShaderState_Vk::~SR_ShaderState_Vk()
{

}

bool SR_ShaderState_Vk::Init(const SR_ShaderStateProperties& aProperties)
{
	return true;
}

VkPipeline SR_ShaderState_Vk::GetPipeline()
{
	return mVkPipeline;
}

#endif