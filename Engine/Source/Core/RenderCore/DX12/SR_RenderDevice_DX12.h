#pragma once
#include "RenderCore/Interface/SR_RenderDevice.h"

#if ENABLE_DX12
#include "SR_CommandQueue_DX12.h"

#define ENABLE_NVIDIA_AFTERMATH (0)
#define ENABLE_NVAPI			(1)
#define ENABLE_AGS				(1)
#define ENABLE_DRED				(0)

struct ID3D12Device;
struct ID3D12Device5;
struct ID3D12Device6;
struct ID3D12RootSignature;

class SR_SwapChain_DX12;
class SR_CommandList;
class SR_Texture_DX12;
struct SR_TextureProperties;
class SR_RenderTarget_DX12;
struct SR_RenderTargetProperties;
class SR_TextureResource_DX12;
struct SR_TextureResourceProperties;
class SR_Buffer_DX12;
class SR_BufferResource_DX12;
class SR_DescriptorHeap_DX12;
class SR_DxcCompiler;

class SR_RenderDevice_DX12 : public SR_RenderDevice
{
public:
	SR_RenderDevice_DX12();
	~SR_RenderDevice_DX12();

	SC_Ref<SR_CommandList> CreateCommandList(const SR_CommandListType& aType) override;

	SC_Ref<SR_Texture> CreateTexture(const SR_TextureProperties& aTextureProperties, const SC_Ref<SR_TextureResource>& aResource) override;
	SC_Ref<SR_RenderTarget> CreateRenderTarget(const SR_RenderTargetProperties& aRenderTargetProperties, const SC_Ref<SR_TextureResource>& aResource) override;
	SC_Ref<SR_DepthStencil> CreateDepthStencil(const SR_DepthStencilProperties& aDepthStencilProperties, const SC_Ref<SR_TextureResource>& aResource) override;
	SC_Ref<SR_TextureResource> CreateTextureResource(const SR_TextureResourceProperties& aTextureResourceProperties, const SR_PixelData* aInitialData = nullptr, uint32 aDataCount = 0) override;

	SC_Ref<SR_Buffer> CreateBuffer(const SR_BufferProperties& aBufferProperties, const SC_Ref<SR_BufferResource>& aResource) override;
	SC_Ref<SR_BufferResource> CreateBufferResource(const SR_BufferResourceProperties& aBufferResourceProperties, const void* aInitialData = nullptr) override;

	SC_Ref<SR_Heap> CreateHeap(const SR_HeapProperties& aHeapProperties);

	SC_Ref<SR_FenceResource> CreateFenceResource() override;

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

	ID3D12Device* GetD3D12Device() const;
	ID3D12Device5* GetD3D12Device5() const;
	ID3D12Device6* GetD3D12Device6() const;

#if ENABLE_DRED
	void OutputDredDebugData();
#endif

	SC_SizeT GetAvailableVRAM() const override;
	SC_SizeT GetUsedVRAM() const override;

	static SR_RenderDevice_DX12* gD3D12Instance;

private:
	bool Init(void* aWindowHandle) override;
	bool CreateDefaultRootSignatures();

	bool InitTempStorage();

	void GatherSupportCaps();

#if ENABLE_NVIDIA_AFTERMATH
	static void GpuCrashDumpCallback(const void* aGpuCrashDump, const uint32 aGpuCrashDumpSize, void* aUserData);
	void OnGpuCrashDump(const void* aGpuCrashDump, const uint32 aGpuCrashDumpSize);
#endif

	SC_Ref<SR_Texture> LoadTextureInternal(const SC_FilePath& aTextureFilePath) override;
	SC_Ref<SR_Texture> LoadTextureFromFile(const char* aTextureFilePath);

	SR_ComPtr<IDXGIFactory1> mDXGIFactory;
	SR_ComPtr<IDXGIAdapter1> mDXGIAdapter;
	SR_ComPtr<IDXGIAdapter3> mDXGIAdapter3;

	SR_ComPtr<ID3D12Device> mD3D12Device;
	SR_ComPtr<ID3D12Device5> mD3D12Device5;
	SR_ComPtr<ID3D12Device6> mD3D12Device6;
	SR_ComPtr<ID3D12InfoQueue> mD3D12InfoQueue;

	SC_UniquePtr<SR_CommandQueue_DX12> mCommandQueues[static_cast<uint32>(SR_CommandListType::COUNT)];

	SR_ComPtr<ID3D12RootSignature> mD3D12RootSignatures[static_cast<uint32>(SR_RootSignatureType::COUNT)];

	SC_Ref<SR_DescriptorHeap_DX12> mBindlessDescriptorHeap;
	SC_Ref<SR_DescriptorHeap_DX12> mSamplerDescriptorHeap;
	SC_Ref<SR_DescriptorHeap_DX12> mRTVDescriptorHeap;
	SC_Ref<SR_DescriptorHeap_DX12> mDSVDescriptorHeap;

	SR_ComPtr<ID3D12Heap> mTempTexturesHeap;
	SR_ComPtr<ID3D12Heap> mTempRenderTargetsHeap;
	SR_ComPtr<ID3D12Heap> mTempUploadHeap;

	SC_UniquePtr<SR_DxcCompiler> mDxcCompiler;

#if ENABLE_DRED
	SC_Mutex mDREDMutex;
	bool mEnableDRED : 1;
#endif
};

inline bool VerifyHRESULT(HRESULT aValue)
{
	switch (aValue)
	{
	case E_INVALIDARG:
		assert(false && "Invalid args passed to D3D12 runtime");
		return false;
	case E_OUTOFMEMORY:
		assert(false && "Out of Memory");
		return false;
	case DXGI_ERROR_DEVICE_REMOVED:
#if ENABLE_DRED
		SR_RenderDevice_DX12::gD3D12Instance->OutputDredDebugData();
#endif
		return false;
	case S_OK:
	case S_FALSE:
		return true;
	default:
		return false;
	}
}

#endif