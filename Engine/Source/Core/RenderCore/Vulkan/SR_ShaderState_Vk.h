#pragma once

#include "RenderCore/Interface/SR_ShaderState.h"

#if SR_ENABLE_VULKAN

class SR_ShaderState_Vk final : public SR_ShaderState 
{
public:
	SR_ShaderState_Vk();
	~SR_ShaderState_Vk();

	bool Init(const SR_ShaderStateProperties& aProperties);

	VkPipeline GetPipeline();

private:
	VkPipeline mVkPipeline;
};

#endif