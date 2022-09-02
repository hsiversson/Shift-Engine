#pragma once
#include "RenderCore/Interface/SR_RenderDevice.h"

#if SR_ENABLE_VULKAN

class SR_DirectXShaderCompiler;
class SR_RenderDevice_Vk final : public SR_RenderDevice
{
public:
	SR_RenderDevice_Vk();
	~SR_RenderDevice_Vk();

	SC_Ref<SR_CommandList> CreateCommandList(const SR_CommandListType& aType) override;

	SC_Ref<SR_Texture> CreateTexture(const SR_TextureProperties& aTextureProperties, const SC_Ref<SR_TextureResource>& aResource) override;
	SC_Ref<SR_RenderTarget> CreateRenderTarget(const SR_RenderTargetProperties& aRenderTargetProperties, const SC_Ref<SR_TextureResource>& aResource) override;
	SC_Ref<SR_DepthStencil> CreateDepthStencil(const SR_DepthStencilProperties& aDepthStencilProperties, const SC_Ref<SR_TextureResource>& aResource) override;
	SC_Ref<SR_TextureResource> CreateTextureResource(const SR_TextureResourceProperties& aTextureResourceProperties, const SR_PixelData* aInitialData = nullptr, uint32 aDataCount = 0) override;

	SC_Ref<SR_Buffer> CreateBuffer(const SR_BufferProperties& aBufferProperties, const SC_Ref<SR_BufferResource>& aResource) override;
	SC_Ref<SR_BufferResource> CreateBufferResource(const SR_BufferResourceProperties& aBufferResourceProperties, const void* aInitialData = nullptr) override;

	SC_Ref<SR_Heap> CreateHeap(const SR_HeapProperties& aHeapProperties);

	bool CompileShader(const SR_ShaderCompileArgs& aArgs, SR_ShaderByteCode& aOutByteCode, SR_ShaderMetaData* aOutMetaData = nullptr) override;
	bool CompileShader(const std::string& aShadercode, const SR_ShaderCompileArgs& aArgs, SR_ShaderByteCode& aOutByteCode, SR_ShaderMetaData* aOutMetaData = nullptr) override;
	SC_Ref<SR_ShaderState> CreateShaderState(const SR_ShaderStateProperties& aProperties) override;

	SC_Ref<SR_SwapChain> CreateSwapChain(const SR_SwapChainProperties& aProperties, void* aNativeWindowHandle) override;

	SR_CommandQueue* GetGraphicsCommandQueue() const override;
	SR_CommandQueue* GetCommandQueue(const SR_CommandListType& aType) const override;

	SR_DescriptorHeap* GetDefaultDescriptorHeap() const override;
	SR_DescriptorHeap* GetSamplerDescriptorHeap() const override;
	SR_DescriptorHeap* GetRTVDescriptorHeap() const override;
	SR_DescriptorHeap* GetDSVDescriptorHeap() const override;

	SC_SizeT GetAvailableVRAM() const override;
	SC_SizeT GetUsedVRAM() const override;

	VkDevice& GetVkDevice();
	VkInstance& GetVkInstance();

	static SR_RenderDevice_Vk* gInstance;

private:
	bool Init(void* aWindowHandle) override;

	bool SelectPhysicalDevice();
	bool SetupLogicalDevice();

	void GatherSupportCaps();

	SC_UniquePtr<SR_DirectXShaderCompiler> mDirectXShaderCompiler;

	VkPhysicalDevice mVkPhysicalDevice;
	VkDevice mVkLogicalDevice;
	VkInstance mVkInstance;
};

#endif
