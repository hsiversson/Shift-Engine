#pragma once
#include "RenderCore/Interface/SR_RenderDevice.h"

#if SR_ENABLE_DX12
#include "SR_CommandQueue_DX12.h"

struct ID3D12Device;
struct ID3D12Device5;
struct ID3D12Device6;
struct ID3D12RootSignature;

class SR_SwapChain_DXGI;
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
class SR_DirectXShaderCompiler;

class SR_RenderDevice_DX12 : public SR_RenderDevice
{
public:
	SR_RenderDevice_DX12();
	~SR_RenderDevice_DX12();

	SC_Ref<SR_CommandList> CreateCommandList(const SR_CommandListType& aType, const char* aDebugName = nullptr) override;

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

	ID3D12Device* GetD3D12Device() const;
	ID3D12Device5* GetD3D12Device5() const;
	ID3D12Device6* GetD3D12Device6() const;

#if SR_ENABLE_DRED
	void OutputDredDebugData();
#endif

	SC_SizeT GetAvailableVRAM() const override;
	SC_SizeT GetUsedVRAM() const override;

	static SR_RenderDevice_DX12* gInstance;
	static bool gUsingNvApi;
	static bool gUsingAGS;
private:
	bool Init(void* aWindowHandle) override;
	bool CreateDefaultRootSignatures();

	void GatherSupportCaps();

#if SR_ENABLE_NVIDIA_AFTERMATH
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

	SC_UniquePtr<SR_DirectXShaderCompiler> mDxcCompiler;

#if SR_ENABLE_DRED
	SC_Mutex mDREDMutex;
	bool mEnableDRED : 1;
#endif
};

inline const char* GetRemovedReasonString(HRESULT aValue)
{
	switch (aValue)
	{
	case DXGI_ERROR_DEVICE_HUNG: return "Device Hung";
	case DXGI_ERROR_DEVICE_RESET: return "Device Reset";
	case DXGI_ERROR_DEVICE_REMOVED: return "Device Removed";
	case DXGI_ERROR_DRIVER_INTERNAL_ERROR: return "Internal Driver Error";
	case DXGI_ERROR_INVALID_CALL: return "Invalid Call";
	case S_OK: return "S_OK";
	}

	return "<unknown value>";
}

inline bool VerifyHRESULT(HRESULT aValue)
{
	bool outputDebugData = false;

	switch (aValue)
	{
	case E_INVALIDARG:
		SC_ERROR("[DX12 ERROR]: Invalid Args");
		return false;
	case E_OUTOFMEMORY:
		SC_ERROR("[DX12 ERROR]: Out Of Memory");
		return false;
	case DXGI_ERROR_INVALID_CALL:
		SC_ERROR("[DX12 ERROR]: Invalid Call");
		return false;
	case DXGI_ERROR_UNSUPPORTED:
		SC_ERROR("[DX12 ERROR]: Unsupported Feature Used");
		return false;
	case DXGI_ERROR_DEVICE_HUNG:
		SC_ERROR("[DXGI ERROR]: Device Hung");
		outputDebugData = true;
		break;
	case DXGI_ERROR_DEVICE_REMOVED:
		SC_ERROR("[DXGI ERROR]: Device Removed");
		SC_ERROR("Removed Reason: {}", GetRemovedReasonString(SR_RenderDevice_DX12::gInstance->GetD3D12Device()->GetDeviceRemovedReason()));
		outputDebugData = true;
		break;
	case S_OK:
	case S_FALSE:
		return true;
	default:
		SC_ERROR("[DX12 ERROR]: Unknown Reason ({:0x})", aValue);
		return false;
	}

	if (outputDebugData)
	{
#if SR_ENABLE_DRED
		//SR_RenderDevice_DX12::gInstance->OutputDredDebugData();
#endif
	}

	SC_ASSERT(false);
	return false;
}

#endif