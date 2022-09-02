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
	bool InitDefault(const SR_ShaderStateProperties& aProperties);
#if SR_ENABLE_MESH_SHADERS
	bool InitAsMeshShader(const SR_ShaderStateProperties& aProperties);
#endif
	bool InitAsComputeShader(const SR_ShaderStateProperties& aProperties);
#if SR_ENABLE_RAYTRACING
	bool InitAsRaytracingShader(const SR_ShaderStateProperties& aProperties);
	void CreateRaytracingShaderTable(const SR_ShaderStateProperties& aProperties);
#endif

private:
	VkPipeline mVkPipeline;
};

#endif