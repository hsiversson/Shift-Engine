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

	SC_Array<VkShaderModule> shaderModules;
	for (uint32 shaderType = 0; shaderType < static_cast<uint32>(SR_ShaderType::COUNT); ++shaderType)
	{
		VkShaderModuleCreateInfo props = {};
		props.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		props.codeSize = aProperties.mShaderByteCodes[shaderType].mSize;
		props.pCode = reinterpret_cast<const uint32*>(aProperties.mShaderByteCodes[shaderType].mByteCode.get());

		VkShaderModule module;
		if (vkCreateShaderModule(SR_RenderDevice_Vk::gInstance->GetVkDevice(), &props, nullptr, &module) != VK_SUCCESS)
		{
			SC_ERROR("Could not create shader module.");
			return false;
		}

		shaderModules.Add(module);
	}

	VkPipelineShaderStageCreateInfo shaderStageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
	shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStageInfo.pName = "";


	

	return true;
}

VkPipeline SR_ShaderState_Vk::GetPipeline()
{
	return mVkPipeline;
}

bool SR_ShaderState_Vk::InitDefault(const SR_ShaderStateProperties& aProperties)
{
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	//pipelineInfo.pStages = shaderStages;

	if (vkCreateGraphicsPipelines(SR_RenderDevice_Vk::gInstance->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mVkPipeline) != VK_SUCCESS)
	{
		SC_ERROR("Could not create graphics pipeline.");
		return false;
	}
}

#if SR_ENABLE_MESH_SHADERS
bool SR_ShaderState_Vk::InitAsMeshShader(const SR_ShaderStateProperties& aProperties)
{
}
#endif //SR_ENABLE_MESH_SHADERS

bool SR_ShaderState_Vk::InitAsComputeShader(const SR_ShaderStateProperties& aProperties)
{
	VkComputePipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	if (vkCreateComputePipelines(SR_RenderDevice_Vk::gInstance->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mVkPipeline) != VK_SUCCESS)
	{
		SC_ERROR("Could not create compute pipeline.");
		return false;
	}
}

#if SR_ENABLE_RAYTRACING
bool SR_ShaderState_Vk::InitAsRaytracingShader(const SR_ShaderStateProperties& aProperties)
{
	VkRayTracingPipelineCreateInfoKHR pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
	if (vkCreateRayTracingPipelinesKHR(SR_RenderDevice_Vk::gInstance->GetVkDevice(), VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mVkPipeline) != VK_SUCCESS)
	{
		SC_ERROR("Could not create raytracing pipeline.");
		return false;
	}
}

void SR_ShaderState_Vk::CreateRaytracingShaderTable(const SR_ShaderStateProperties& aProperties)
{

}
#endif //SR_ENABLE_RAYTRACING

#endif