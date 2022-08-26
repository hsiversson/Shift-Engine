#include "SR_RenderDevice_Vk.h"
#include "RenderCore/ShaderCompiler/SR_DirectXShaderCompiler.h"

#if SR_ENABLE_VULKAN

SR_RenderDevice_Vk* SR_RenderDevice_Vk::gInstance = nullptr;

SR_RenderDevice_Vk::SR_RenderDevice_Vk()
	: SR_RenderDevice(SR_API::Vulkan)
{
	SC_ASSERT(gInstance, "Already created RenderDevice.");
	gInstance = this;
}

SR_RenderDevice_Vk::~SR_RenderDevice_Vk()
{

}

SC_Ref<SR_CommandList> SR_RenderDevice_Vk::CreateCommandList(const SR_CommandListType& /*aType*/)
{

}

SC_Ref<SR_Texture> SR_RenderDevice_Vk::CreateTexture(const SR_TextureProperties& /*aTextureProperties*/, const SC_Ref<SR_TextureResource>& /*aResource*/)
{

}

SC_Ref<SR_RenderTarget> SR_RenderDevice_Vk::CreateRenderTarget(const SR_RenderTargetProperties& /*aRenderTargetProperties*/, const SC_Ref<SR_TextureResource>& /*aResource*/)
{

}

SC_Ref<SR_DepthStencil> SR_RenderDevice_Vk::CreateDepthStencil(const SR_DepthStencilProperties& /*aDepthStencilProperties*/, const SC_Ref<SR_TextureResource>& /*aResource*/)
{

}

SC_Ref<SR_TextureResource> SR_RenderDevice_Vk::CreateTextureResource(const SR_TextureResourceProperties& /*aTextureResourceProperties*/, const SR_PixelData* /*aInitialData*/ /*= nullptr*/, uint32 /*aDataCount*/ /*= 0*/)
{

}

SC_Ref<SR_Buffer> SR_RenderDevice_Vk::CreateBuffer(const SR_BufferProperties& /*aBufferProperties*/, const SC_Ref<SR_BufferResource>& /*aResource*/)
{

}

SC_Ref<SR_BufferResource> SR_RenderDevice_Vk::CreateBufferResource(const SR_BufferResourceProperties& /*aBufferResourceProperties*/, const void* /*aInitialData*/ /*= nullptr*/)
{

}

SC_Ref<SR_Heap> SR_RenderDevice_Vk::CreateHeap(const SR_HeapProperties& /*aHeapProperties*/)
{

}

bool SR_RenderDevice_Vk::CompileShader(const SR_ShaderCompileArgs& /*aArgs*/, SR_ShaderByteCode& /*aOutByteCode*/, SR_ShaderMetaData* /*aOutMetaData*/ /*= nullptr*/)
{

}

bool SR_RenderDevice_Vk::CompileShader(const std::string& /*aShadercode*/, const SR_ShaderCompileArgs& /*aArgs*/, SR_ShaderByteCode& /*aOutByteCode*/, SR_ShaderMetaData* /*aOutMetaData*/ /*= nullptr*/)
{

}

SC_Ref<SR_ShaderState> SR_RenderDevice_Vk::CreateShaderState(const SR_ShaderStateProperties& /*aProperties*/)
{

}

SC_Ref<SR_SwapChain> SR_RenderDevice_Vk::CreateSwapChain(const SR_SwapChainProperties& /*aProperties*/, void* /*aNativeWindowHandle*/)
{

}

SR_CommandQueue* SR_RenderDevice_Vk::GetGraphicsCommandQueue() const
{

}

SR_CommandQueue* SR_RenderDevice_Vk::GetCommandQueue(const SR_CommandListType& /*aType*/) const
{

}

SR_DescriptorHeap* SR_RenderDevice_Vk::GetDefaultDescriptorHeap() const
{

}

SR_DescriptorHeap* SR_RenderDevice_Vk::GetSamplerDescriptorHeap() const
{

}

SR_DescriptorHeap* SR_RenderDevice_Vk::GetRTVDescriptorHeap() const
{

}

SR_DescriptorHeap* SR_RenderDevice_Vk::GetDSVDescriptorHeap() const
{

}

SC_SizeT SR_RenderDevice_Vk::GetAvailableVRAM() const
{

}

SC_SizeT SR_RenderDevice_Vk::GetUsedVRAM() const
{

}

bool SR_RenderDevice_Vk::Init(void* /*aWindowHandle*/)
{

	VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
	appInfo.apiVersion = VK_API_VERSION_1_3;
	appInfo.pEngineName = "Shift Engine";
	appInfo.engineVersion = 1;
	appInfo.pApplicationName = "Example Game";
	appInfo.applicationVersion = 1;

	SC_Array<const char*> extensions;
	SC_Array<const char*> layers;

	VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledExtensionCount = extensions.Count();
	instanceCreateInfo.ppEnabledExtensionNames = extensions.GetBuffer();
	instanceCreateInfo.enabledLayerCount = layers.Count();
	instanceCreateInfo.ppEnabledLayerNames = layers.GetBuffer();

	VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &mVkInstance);
	if (result != VK_SUCCESS)
	{
		SC_ERROR("Failed to create VkInstance.");
		return false;
	}

	if (!SelectPhysicalDevice())
		return false;

	GatherSupportCaps();

	if (!SetupLogicalDevice())
		return false;

	mDirectXShaderCompiler = SC_MakeUnique<SR_DirectXShaderCompiler>(SR_DirectXShaderCompiler::Backend::SPIRV);

	return true;
}

bool SR_RenderDevice_Vk::SelectPhysicalDevice()
{
	return false;
}

bool SR_RenderDevice_Vk::SetupLogicalDevice()
{
	return false;
}

void SR_RenderDevice_Vk::GatherSupportCaps()
{

}

#endif