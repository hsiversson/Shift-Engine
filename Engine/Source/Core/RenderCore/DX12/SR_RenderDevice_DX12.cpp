
#include "SR_RenderDevice_DX12.h"

#if ENABLE_DX12

// D3D12 Agility SDK
extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 4; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8"./"; }
////////////////////

#include "SR_SwapChain_DX12.h"
#include "SR_CommandQueue_DX12.h"
#include "SR_Texture_DX12.h"
#include "SR_RenderTarget_DX12.h"
#include "SR_DepthStencil_DX12.h"
#include "SR_TextureResource_DX12.h"
#include "SR_DescriptorHeap_DX12.h"
#include "SR_BufferResource_DX12.h"
#include "SR_Heap_DX12.h"
#include "SR_ShaderState_DX12.h"
#include "SR_Buffer_DX12.h"
#include "SR_RootSignature_DX12.h"
#include "RenderCore/Resources/SR_TextureLoading.h"
#include "RenderCore/ShaderCompiler/SR_DxcCompiler.h"

#define USE_D3D12_DDS_LOADER (1)
#if USE_D3D12_DDS_LOADER
	#include "RenderCore/Resources/DDSTextureLoader.h"
#endif

#if IS_DESKTOP_PLATFORM
	#if ENABLE_NVIDIA_AFTERMATH
		#include "nvapi.h"
	#endif
	#if ENABLE_NVIDIA_AFTERMATH
		#include "amd_ags.h"
	#endif

	#if ENABLE_NVIDIA_AFTERMATH
		#include "GFSDK_Aftermath.h"
	#endif
#endif

SR_RenderDevice_DX12* SR_RenderDevice_DX12::gD3D12Instance = nullptr;

SR_RenderDevice_DX12::SR_RenderDevice_DX12()
	: SR_RenderDevice(SR_API::D3D12)
	, mEnableDRED(false)
{
	if (gD3D12Instance != nullptr)
		SC_ASSERT(false, "Already created RenderDevice.");
	else
		gD3D12Instance = this;

	if (SC_CommandLine::HasCommand("dred"))
		mEnableDRED = true;

	SC_Fill(mD3D12RootSignatures, static_cast<uint32>(SR_RootSignatureType::COUNT), nullptr);
}

SR_RenderDevice_DX12::~SR_RenderDevice_DX12()
{
}

SC_Ref<SR_CommandList> SR_RenderDevice_DX12::CreateCommandList(const SR_CommandListType& aType)
{
	SC_Ref<SR_CommandList_DX12> cmdList = SC_MakeRef<SR_CommandList_DX12>(aType);

	if (!cmdList->Init())
	{
		return nullptr;
	}

	return cmdList;
}

SC_Ref<SR_Texture> SR_RenderDevice_DX12::CreateTexture(const SR_TextureProperties& aTextureProperties, const SC_Ref<SR_TextureResource>& aResource)
{
	SC_Ref<SR_Texture_DX12> newTexture = SC_MakeRef<SR_Texture_DX12>(aTextureProperties, aResource);

	if (!newTexture->Init())
	{
		return nullptr;
	}

	return newTexture;
}

SC_Ref<SR_RenderTarget> SR_RenderDevice_DX12::CreateRenderTarget(const SR_RenderTargetProperties& aRenderTargetProperties, const SC_Ref<SR_TextureResource>& aResource)
{
	SC_Ref<SR_RenderTarget_DX12> newRenderTarget = SC_MakeRef<SR_RenderTarget_DX12>(aRenderTargetProperties, aResource);

	if (!newRenderTarget->Init())
	{
		return nullptr;
	}

	return newRenderTarget;
}

SC_Ref<SR_DepthStencil> SR_RenderDevice_DX12::CreateDepthStencil(const SR_DepthStencilProperties& aDepthStencilProperties, const SC_Ref<SR_TextureResource>& aResource)
{
	SC_Ref<SR_DepthStencil_DX12> newDepthStencil = SC_MakeRef<SR_DepthStencil_DX12>(aDepthStencilProperties, aResource);

	if (!newDepthStencil->Init())
	{
		return nullptr;
	}

	return newDepthStencil;
}

SC_Ref<SR_TextureResource> SR_RenderDevice_DX12::CreateTextureResource(const SR_TextureResourceProperties& aTextureResourceProperties, const SR_PixelData* aInitialData, uint32 aDataCount)
{
	SC_Ref<SR_TextureResource_DX12> newTextureResource = SC_MakeRef<SR_TextureResource_DX12>(aTextureResourceProperties);

	if (!newTextureResource->Init(aInitialData, aDataCount))
	{
		return nullptr;		
	}

	return newTextureResource;
}

SC_Ref<SR_Buffer> SR_RenderDevice_DX12::CreateBuffer(const SR_BufferProperties& aBufferProperties, const SC_Ref<SR_BufferResource>& aResource)
{
	SC_Ref<SR_Buffer_DX12> newBuffer = SC_MakeRef<SR_Buffer_DX12>(aBufferProperties, aResource);

	if (!newBuffer->Init())
	{
		return nullptr;
	}

	return newBuffer;
}

SC_Ref<SR_BufferResource> SR_RenderDevice_DX12::CreateBufferResource(const SR_BufferResourceProperties& aBufferResourceProperties, const void* aInitialData)
{
	SC_Ref<SR_BufferResource_DX12> newBufferResource = SC_MakeRef<SR_BufferResource_DX12>(aBufferResourceProperties);

	if (!newBufferResource->Init(aInitialData))
	{
		return nullptr;
	}

	return newBufferResource;
}

SC_Ref<SR_Heap> SR_RenderDevice_DX12::CreateHeap(const SR_HeapProperties& aHeapProperties)
{
	SC_Ref<SR_Heap_DX12> newHeap = SC_MakeRef<SR_Heap_DX12>(aHeapProperties);
	if (!newHeap->Init())
	{
		return nullptr;
	}

	return newHeap;
}

bool SR_RenderDevice_DX12::CompileShader(const SR_ShaderCompileArgs& aArgs, SR_ShaderByteCode& aOutByteCode, SR_ShaderMetaData* aOutMetaData)
{
	return mDxcCompiler->CompileFromFile(aArgs, aOutByteCode, aOutMetaData);
}

bool SR_RenderDevice_DX12::CompileShader(const std::string& aShadercode, const SR_ShaderCompileArgs& aArgs, SR_ShaderByteCode& aOutByteCode, SR_ShaderMetaData* aOutMetaData)
{
	return mDxcCompiler->CompileFromString(aShadercode, aArgs, aOutByteCode, aOutMetaData);
}

SC_Ref<SR_ShaderState> SR_RenderDevice_DX12::CreateShaderState(const SR_ShaderStateProperties& aProperties)
{
	SC_Ref<SR_ShaderState_DX12> newShaderState = SC_MakeRef<SR_ShaderState_DX12>();

	if (!newShaderState->Init(aProperties))
	{
		return nullptr;
	}

	return newShaderState;
}

SC_Ref<SR_SwapChain> SR_RenderDevice_DX12::CreateSwapChain(const SR_SwapChainProperties& aProperties, void* aNativeWindowHandle)
{
	SC_Ref<SR_SwapChain_DX12> newSwapChain = SC_MakeRef<SR_SwapChain_DX12>(this);

	if (!newSwapChain->Init(aProperties, aNativeWindowHandle))
		return nullptr;

	return newSwapChain;
}

SR_CommandQueue* SR_RenderDevice_DX12::GetGraphicsCommandQueue() const
{
	return mCommandQueues[static_cast<uint32>(SR_CommandListType::Graphics)].get();
}

SR_CommandQueue* SR_RenderDevice_DX12::GetCommandQueue(const SR_CommandListType& aType) const
{
	return mCommandQueues[static_cast<uint32>(aType)].get();
}

SR_DescriptorHeap* SR_RenderDevice_DX12::GetDefaultDescriptorHeap() const
{
	return mBindlessDescriptorHeap.get();
}

SR_DescriptorHeap* SR_RenderDevice_DX12::GetSamplerDescriptorHeap() const
{
	return mSamplerDescriptorHeap.get();
}

SR_DescriptorHeap* SR_RenderDevice_DX12::GetRTVDescriptorHeap() const
{
	return mRTVDescriptorHeap.get();
}

SR_DescriptorHeap* SR_RenderDevice_DX12::GetDSVDescriptorHeap() const
{
	return mDSVDescriptorHeap.get();
}

ID3D12Device* SR_RenderDevice_DX12::GetD3D12Device() const
{
	return mD3D12Device.Get();
}

ID3D12Device5* SR_RenderDevice_DX12::GetD3D12Device5() const
{
	return mD3D12Device5.Get();
}

ID3D12Device6* SR_RenderDevice_DX12::GetD3D12Device6() const
{
	return mD3D12Device6.Get();
}

void SR_RenderDevice_DX12::OutputDredDebugData()
{
	SR_ComPtr<ID3D12DeviceRemovedExtendedData1> dred;
	VerifyHRESULT(mD3D12Device->QueryInterface(IID_PPV_ARGS(&dred)));

	D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT1 dredAutoBreadcrumbsOutput;
	VerifyHRESULT(dred->GetAutoBreadcrumbsOutput1(&dredAutoBreadcrumbsOutput));

	D3D12_DRED_PAGE_FAULT_OUTPUT dredPageFaultOutput;
	VerifyHRESULT(dred->GetPageFaultAllocationOutput(&dredPageFaultOutput));
}

bool SR_RenderDevice_DX12::Init(void* /*aWindowHandle*/)
{
	SR_ComPtr<IDXGIFactory1> dxgiFactory;
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

	SR_ComPtr<IDXGIAdapter1> dxgiAdapter;
	SR_ComPtr<IDXGIAdapter1> tempAdapter;
	DXGI_ADAPTER_DESC1 adapterDesc = {};
	for (uint32 i = 0; dxgiFactory->EnumAdapters1(i, &tempAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC1 desc = {};
		tempAdapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		if (desc.DedicatedVideoMemory > adapterDesc.DedicatedVideoMemory)
		{
			adapterDesc = desc;
			dxgiAdapter = tempAdapter;
		}
	}

	{
		if (adapterDesc.VendorId == 0x10DE)
		{
			mSupportCaps.mUsingNvidiaGPU = true;
			SC_LOG("Graphics Vendor: Nvidia");
		}
		else if (adapterDesc.VendorId == 0x1002)
		{
			mSupportCaps.mUsingAmdGPU = true;
			SC_LOG("Graphics Vendor: AMD");
		}
		else if (adapterDesc.VendorId == 0x163C || adapterDesc.VendorId == 0x8086)
		{
			mSupportCaps.mUsingIntelGPU = true;
			SC_LOG("Graphics Vendor: Intel");
		}

		std::string deviceName = SC_UTF16ToUTF8(adapterDesc.Description);

		SC_LOG("Graphics card: %s (id: %x rev: %x)", deviceName.c_str(), adapterDesc.DeviceId, adapterDesc.Revision);
		SC_LOG("Video Memory: %lluMB", uint64(adapterDesc.DedicatedVideoMemory >> 20));

#if IS_DESKTOP_PLATFORM
#if ENABLE_NVAPI
		if (mSupportCaps.mUsingNvidiaGPU)
		{
			NvAPI_Status status = NvAPI_Initialize();
			if (status == NVAPI_OK)
			{
				NvU32 driverVersion;
				NvAPI_ShortString driverString;
				status = NvAPI_SYS_GetDriverAndBranchVersion(&driverVersion, driverString);
				if (status != NVAPI_OK)
				{
					NvAPI_ShortString string;
					NvAPI_GetErrorMessage(status, string);
					SC_ERROR("NvAPI_SYS_GetDriverAndBranchVersion: %s", string);
				}

				SC_LOG("Nvidia Driver: %d (Driver String: %s)", driverVersion, driverString);
			}
		}
#endif //ENABLE_NVAPI
#if ENABLE_AGS
		else if (mSupportCaps.mUsingAmdGPU)
		{
			AGSConfiguration config = {};
			AGSGPUInfo gpuInfo = {};
			AGSReturnCode status = agsInitialize(AGS_MAKE_VERSION(AMD_AGS_VERSION_MAJOR, AMD_AGS_VERSION_MINOR, AMD_AGS_VERSION_PATCH), &config, nullptr, &gpuInfo);
			if (status == AGS_SUCCESS)
			{

			}
		}
#endif //ENABLE_AGS
#endif //IS_DESKTOP_PLATFORM 
	}

	if (mEnableDebugMode)
	{
		SC_LOG("D3D12 Debug Layer: Enabled");
		SR_ComPtr<ID3D12Debug> debugger;
		D3D12GetDebugInterface(IID_PPV_ARGS(&debugger));
		debugger->EnableDebugLayer();
	}

	if (mEnableDRED)
	{
		SC_LOG("D3D12 DRED: Enabled");
		SR_ComPtr<ID3D12DeviceRemovedExtendedDataSettings1> dredSettings;
		D3D12GetDebugInterface(IID_PPV_ARGS(&dredSettings));
		dredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		dredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
	}

	SC_LOG("Creating D3D12Device.");
	hr = D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mD3D12Device));


#if IS_DESKTOP_PLATFORM
#if ENABLE_NVIDIA_AFTERMATH
	if (SC_CommandLine::HasCommand("enableaftermath"))
	{
		const uint32 aftermathFlags =
			GFSDK_Aftermath_FeatureFlags_EnableMarkers |           // Enable event marker tracking.
			GFSDK_Aftermath_FeatureFlags_EnableResourceTracking |  // Enable tracking of resources.
			GFSDK_Aftermath_FeatureFlags_CallStackCapturing |      // Capture call stacks for all draw calls, compute dispatches, and resource copies.
			GFSDK_Aftermath_FeatureFlags_GenerateShaderDebugInfo;  // Generate debug information for shaders.

		GFSDK_Aftermath_DX12_Initialize(GFSDK_Aftermath_Version_API, aftermathFlags, mD3D12Device.Get());
	}

#endif //ENABLE_NVIDIA_AFTERMATH
#endif


	hr = mD3D12Device.As(&mD3D12Device5);
	hr = mD3D12Device.As(&mD3D12Device6);
	GatherSupportCaps();

	if (mEnableDebugMode)
	{
		if (SUCCEEDED(mD3D12Device->QueryInterface(IID_PPV_ARGS(&mD3D12InfoQueue))))
		{
			D3D12_MESSAGE_SEVERITY severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};
			D3D12_MESSAGE_ID denyIds[] =
			{
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE
			};

			D3D12_INFO_QUEUE_FILTER newFilter = {};
			newFilter.DenyList.NumSeverities = _countof(severities);
			newFilter.DenyList.pSeverityList = severities;
			newFilter.DenyList.NumIDs = _countof(denyIds);
			newFilter.DenyList.pIDList = denyIds;
			mD3D12InfoQueue->PushStorageFilter(&newFilter);
			if (mBreakOnError)
			{
				mD3D12InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
				mD3D12InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			}
		}
	}

	mCommandQueues[static_cast<uint32>(SR_CommandListType::Graphics)] = SC_MakeUnique<SR_CommandQueue_DX12>(this);
	if (!mCommandQueues[static_cast<uint32>(SR_CommandListType::Graphics)]->Init(SR_CommandListType::Graphics))
		return false;

	mCommandQueues[static_cast<uint32>(SR_CommandListType::Compute)] = SC_MakeUnique<SR_CommandQueue_DX12>(this);
	if (!mCommandQueues[static_cast<uint32>(SR_CommandListType::Compute)]->Init(SR_CommandListType::Compute))
		return false;

	mCommandQueues[static_cast<uint32>(SR_CommandListType::Copy)] = SC_MakeUnique<SR_CommandQueue_DX12>(this);
	if (!mCommandQueues[static_cast<uint32>(SR_CommandListType::Copy)]->Init(SR_CommandListType::Copy))
		return false;

	// Create Descriptor heaps
	mBindlessDescriptorHeap = SC_MakeRef<SR_DescriptorHeap_DX12>(8192, SR_DescriptorHeapType::CBV_SRV_UAV, true);
	mSamplerDescriptorHeap = SC_MakeRef<SR_DescriptorHeap_DX12>(128, SR_DescriptorHeapType::SAMPLER, true);
	mRTVDescriptorHeap = SC_MakeRef<SR_DescriptorHeap_DX12>(512, SR_DescriptorHeapType::RTV);
	mDSVDescriptorHeap = SC_MakeRef<SR_DescriptorHeap_DX12>(32, SR_DescriptorHeapType::DSV);

	CreateDefaultRootSignatures(); 
	if (!InitTempStorage())
		return false;

	mDxcCompiler = SC_MakeUnique<SR_DxcCompiler>();

	return PostInit();
}

bool SR_RenderDevice_DX12::CreateDefaultRootSignatures()
{
	SR_RootSignatureProperties properties;

	// 2 root cbvs
	for (uint32 i = 0; i < 2; ++i)
	{
		SR_RootParam& param = properties.mRootParams.Add();
		param.InitAsDescriptor(i, 0, SR_RootParamVisibility::All, SR_RootParamType::CBV);
	}

	SR_StaticSamplerProperties samplerProps;
	samplerProps.mVisibility = SR_RootParamVisibility::All;
	samplerProps.mProperties.mBorderColor = SC_Vector4(0);
	samplerProps.mProperties.mComparison = SR_ComparisonFunc::Never;
	samplerProps.mProperties.mLODBias = 0.f;
	samplerProps.mProperties.mMaxAnisotropy = 0;
	samplerProps.mProperties.mMinFilter = SR_FilterMode::Point;
	samplerProps.mProperties.mMagFilter = SR_FilterMode::Point;
	samplerProps.mProperties.mMipFilter = SR_FilterMode::Point;
	samplerProps.mProperties.mWrapX = SR_WrapMode::Clamp;
	samplerProps.mProperties.mWrapY = SR_WrapMode::Clamp;
	samplerProps.mProperties.mWrapZ = SR_WrapMode::Clamp;
	properties.mStaticSamplers.Add(samplerProps);  // Point Clamp

	samplerProps.mProperties.mWrapX = SR_WrapMode::Wrap;
	samplerProps.mProperties.mWrapY = SR_WrapMode::Wrap;
	samplerProps.mProperties.mWrapZ = SR_WrapMode::Wrap;
	properties.mStaticSamplers.Add(samplerProps); // Point Wrap

	samplerProps.mProperties.mWrapX = SR_WrapMode::Mirror;
	samplerProps.mProperties.mWrapY = SR_WrapMode::Mirror;
	samplerProps.mProperties.mWrapZ = SR_WrapMode::Mirror;
	properties.mStaticSamplers.Add(samplerProps); // Point Mirror

	samplerProps.mProperties.mMinFilter = SR_FilterMode::Linear;
	samplerProps.mProperties.mMagFilter = SR_FilterMode::Linear;
	samplerProps.mProperties.mMipFilter = SR_FilterMode::Point;
	samplerProps.mProperties.mWrapX = SR_WrapMode::Clamp;
	samplerProps.mProperties.mWrapY = SR_WrapMode::Clamp;
	samplerProps.mProperties.mWrapZ = SR_WrapMode::Clamp;
	properties.mStaticSamplers.Add(samplerProps);  // Bilinear Clamp

	samplerProps.mProperties.mWrapX = SR_WrapMode::Wrap;
	samplerProps.mProperties.mWrapY = SR_WrapMode::Wrap;
	samplerProps.mProperties.mWrapZ = SR_WrapMode::Wrap;
	properties.mStaticSamplers.Add(samplerProps); // Bilinear Wrap

	samplerProps.mProperties.mWrapX = SR_WrapMode::Mirror;
	samplerProps.mProperties.mWrapY = SR_WrapMode::Mirror;
	samplerProps.mProperties.mWrapZ = SR_WrapMode::Mirror;
	properties.mStaticSamplers.Add(samplerProps); // Bilinear Mirror

	samplerProps.mProperties.mMinFilter = SR_FilterMode::Linear;
	samplerProps.mProperties.mMagFilter = SR_FilterMode::Linear;
	samplerProps.mProperties.mMipFilter = SR_FilterMode::Linear;
	samplerProps.mProperties.mWrapX = SR_WrapMode::Clamp;
	samplerProps.mProperties.mWrapY = SR_WrapMode::Clamp;
	samplerProps.mProperties.mWrapZ = SR_WrapMode::Clamp;
	properties.mStaticSamplers.Add(samplerProps);  // Trilinear Clamp

	samplerProps.mProperties.mWrapX = SR_WrapMode::Wrap;
	samplerProps.mProperties.mWrapY = SR_WrapMode::Wrap;
	samplerProps.mProperties.mWrapZ = SR_WrapMode::Wrap;
	properties.mStaticSamplers.Add(samplerProps); // Trilinear Wrap

	samplerProps.mProperties.mWrapX = SR_WrapMode::Mirror;
	samplerProps.mProperties.mWrapY = SR_WrapMode::Mirror;
	samplerProps.mProperties.mWrapZ = SR_WrapMode::Mirror;
	properties.mStaticSamplers.Add(samplerProps); // Trilinear Mirror

	samplerProps.mProperties.mMinFilter = SR_FilterMode::Linear;
	samplerProps.mProperties.mMagFilter = SR_FilterMode::Linear;
	samplerProps.mProperties.mMipFilter = SR_FilterMode::Linear;
	samplerProps.mProperties.mMaxAnisotropy = 16;
	samplerProps.mProperties.mWrapX = SR_WrapMode::Clamp;
	samplerProps.mProperties.mWrapY = SR_WrapMode::Clamp;
	samplerProps.mProperties.mWrapZ = SR_WrapMode::Clamp;
	properties.mStaticSamplers.Add(samplerProps);  // Anisotropic Clamp

	samplerProps.mProperties.mWrapX = SR_WrapMode::Wrap;
	samplerProps.mProperties.mWrapY = SR_WrapMode::Wrap;
	samplerProps.mProperties.mWrapZ = SR_WrapMode::Wrap;
	properties.mStaticSamplers.Add(samplerProps); // Anisotropic Wrap

	samplerProps.mProperties.mWrapX = SR_WrapMode::Mirror;
	samplerProps.mProperties.mWrapY = SR_WrapMode::Mirror;
	samplerProps.mProperties.mWrapZ = SR_WrapMode::Mirror;
	properties.mStaticSamplers.Add(samplerProps); // Anisotropic Mirror

	samplerProps.mProperties.mMinFilter = SR_FilterMode::Linear;
	samplerProps.mProperties.mMagFilter = SR_FilterMode::Linear;
	samplerProps.mProperties.mMipFilter = SR_FilterMode::Point;
	samplerProps.mProperties.mWrapX = SR_WrapMode::Border;
	samplerProps.mProperties.mWrapY = SR_WrapMode::Border;
	samplerProps.mProperties.mWrapZ = SR_WrapMode::Border;
	samplerProps.mProperties.mBorderColor = SC_Vector4(1.0f);
	samplerProps.mProperties.mMaxAnisotropy = 0;
	samplerProps.mProperties.mComparison = SR_ComparisonFunc::GreaterEqual;
	properties.mStaticSamplers.Add(samplerProps); // Shadow Sampler

	{
		properties.mFlags = SR_RootSignatureFlag_DefaultSetup;
		properties.mFlags |= SR_RootSignatureFlag_DescriptorHeapDirectAccess;
		properties.mFlags |= SR_RootSignatureFlag_SamplerHeapDirectAccess;
		SC_Ref<SR_RootSignature_DX12> rootSig = SC_MakeRef<SR_RootSignature_DX12>(properties);

		if (!rootSig->Init())
			return false;

		mRootSignatures[static_cast<uint32>(SR_RootSignatureType::Graphics)] = rootSig;
	}
	{
		properties.mFlags = SR_RootSignatureFlag_MeshShaderSetup;
		properties.mFlags |= SR_RootSignatureFlag_DescriptorHeapDirectAccess;
		properties.mFlags |= SR_RootSignatureFlag_SamplerHeapDirectAccess;
		SC_Ref<SR_RootSignature_DX12> rootSig = SC_MakeRef<SR_RootSignature_DX12>(properties);

		if (!rootSig->Init())
			return false;

		mRootSignatures[static_cast<uint32>(SR_RootSignatureType::GraphicsMS)] = rootSig;
	}
	{
		properties.mFlags = SR_RootSignatureFlag_ComputeSetup;
		properties.mFlags |= SR_RootSignatureFlag_DescriptorHeapDirectAccess;
		properties.mFlags |= SR_RootSignatureFlag_SamplerHeapDirectAccess;
		properties.mIsCompute = true;
		SC_Ref<SR_RootSignature_DX12> rootSig = SC_MakeRef<SR_RootSignature_DX12>(properties);

		if (!rootSig->Init())
			return false;

		mRootSignatures[static_cast<uint32>(SR_RootSignatureType::Compute)] = rootSig;
	}
#if ENABLE_RAYTRACING
	{
		properties.mFlags = SR_RootSignatureFlag_ComputeSetup;
		properties.mFlags |= SR_RootSignatureFlag_DescriptorHeapDirectAccess;
		properties.mFlags |= SR_RootSignatureFlag_SamplerHeapDirectAccess;
		properties.mIsCompute = true;
		SC_Ref<SR_RootSignature_DX12> rootSig = SC_MakeRef<SR_RootSignature_DX12>(properties);

		if (!rootSig->Init())
			return false;
		mRootSignatures[static_cast<uint32>(SR_RootSignatureType::Raytracing)] = rootSig;
	}
#endif

	return true;
}

bool SR_RenderDevice_DX12::InitTempStorage()
{
	D3D12_HEAP_DESC desc = {};
	desc.SizeInBytes = MB(512);
	desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
	//HRESULT hr = mD3D12Device->CreateHeap(&desc, IID_PPV_ARGS(&mTempTexturesHeap));
	//if (FAILED(hr))
	//	return false;

	desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
	HRESULT hr = mD3D12Device->CreateHeap(&desc, IID_PPV_ARGS(&mTempRenderTargetsHeap));
	if (FAILED(hr))
		return false;

	desc.SizeInBytes = MB(256);
	desc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
	desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
	hr = mD3D12Device->CreateHeap(&desc, IID_PPV_ARGS(&mTempUploadHeap));

	return SUCCEEDED(hr);
}

static const char* GetShaderModelString(const SR_ShaderModel& aShaderModel)
{
	switch (aShaderModel)
	{
	case SR_ShaderModel::SM_5_1:
		return "5.1";
	case SR_ShaderModel::SM_6_0:
		return "6.0";
	case SR_ShaderModel::SM_6_1:
		return "6.1";
	case SR_ShaderModel::SM_6_2:
		return "6.2";
	case SR_ShaderModel::SM_6_3:
		return "6.3";
	case SR_ShaderModel::SM_6_4:
		return "6.4";
	case SR_ShaderModel::SM_6_5:
		return "6.5";
	case SR_ShaderModel::SM_6_6:
		return "6.6";
	case SR_ShaderModel::SM_6_7:
		return "6.7";
	default:
		return "Invalid";
	}
}

void SR_RenderDevice_DX12::GatherSupportCaps()
{
	mSupportCaps.mEnableAsyncCompute = true;
	if (SC_CommandLine::HasCommand("noasynccompute"))
		mSupportCaps.mEnableAsyncCompute = false;
	SC_LOG("Asynchronous Compute: %s", mSupportCaps.mEnableAsyncCompute ? "true" : "false");

	D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {};
	shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_7;
	HRESULT hr = mD3D12Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
	while (hr == E_INVALIDARG && shaderModel.HighestShaderModel > D3D_SHADER_MODEL_6_0)
	{
		shaderModel.HighestShaderModel = D3D_SHADER_MODEL(shaderModel.HighestShaderModel - 1);
		hr = mD3D12Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
	}
	mSupportCaps.mHighestShaderModel = SR_D3D12ConvertShaderModel(shaderModel.HighestShaderModel);
	SC_LOG("Shader Model: %s", GetShaderModelString(mSupportCaps.mHighestShaderModel));

	D3D12_FEATURE_DATA_D3D12_OPTIONS dx12Options = {};
	hr = mD3D12Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &dx12Options, sizeof(dx12Options));
	if (FAILED(hr))
	{
		SC_ERROR("Could not check options from device");
		return;
	}

	mSupportCaps.mEnableConservativeRasterization = dx12Options.ConservativeRasterizationTier > D3D12_CONSERVATIVE_RASTERIZATION_TIER_NOT_SUPPORTED;
	SC_LOG("Conservative Rasterization: %s", mSupportCaps.mEnableConservativeRasterization ? "true" : "false");

	D3D12_FEATURE_DATA_D3D12_OPTIONS5 dx12Options5 = {};
	hr = mD3D12Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &dx12Options5, sizeof(dx12Options5));
	if (FAILED(hr))
	{
		SC_ERROR("Could not check Options5 from device");
		return;
	}

	mSupportCaps.mEnableRaytracing = dx12Options5.RaytracingTier > D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
	SC_LOG("Raytracing: %s", mSupportCaps.mEnableRaytracing ? "true" : "false");

	D3D12_FEATURE_DATA_D3D12_OPTIONS6 dx12Options6 = {};
	hr = mD3D12Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &dx12Options6, sizeof(dx12Options6));
	if (FAILED(hr))
	{
		SC_ERROR("Could not check Options6 from device");
		return;
	}

	mSupportCaps.mEnableVRS = dx12Options6.VariableShadingRateTier > D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED;
	SC_LOG("Variable Rate Shading: %s", mSupportCaps.mEnableVRS ? "true" : "false");

	D3D12_FEATURE_DATA_D3D12_OPTIONS7 dx12Options7 = {};
	hr = mD3D12Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &dx12Options7, sizeof(dx12Options7));
	if (FAILED(hr))
	{
		SC_ERROR("Could not check Options7 from device");
		return;
	}

	mSupportCaps.mEnableMeshShaders = dx12Options7.MeshShaderTier > D3D12_MESH_SHADER_TIER_NOT_SUPPORTED;
	mSupportCaps.mEnableSamplerFeedback = dx12Options7.SamplerFeedbackTier > D3D12_SAMPLER_FEEDBACK_TIER_NOT_SUPPORTED;
	//float meshShadersTier = dx12Options7.MeshShaderTier / 10.f;
	SC_LOG("Mesh Shaders: %s", mSupportCaps.mEnableMeshShaders ? "true" : "false");
	SC_LOG("Sampler Feedback: %s", mSupportCaps.mEnableSamplerFeedback ? "true" : "false");
}

SC_Ref<SR_Texture> SR_RenderDevice_DX12::LoadTextureInternal(const SC_FilePath& aTextureFilePath)
{
	SR_TextureResourceProperties props;

#if USE_D3D12_DDS_LOADER
	std::vector<D3D12_SUBRESOURCE_DATA> subresourceData;
	std::unique_ptr<uint8[]> data;
	ID3D12Resource* res;
	DirectX::LoadDDSTextureFromFile(mD3D12Device.Get(), SC_UTF8ToUTF16(aTextureFilePath.GetAbsolutePath().c_str()).c_str(), &res, data, subresourceData, 0, nullptr, &props.mIsCubeMap);

	D3D12_RESOURCE_DESC desc = res->GetDesc();

	props.mSize.x = (int)desc.Width;
	props.mSize.y = (int)desc.Height;
	props.mNumMips = desc.MipLevels;
	props.mFormat = SR_D3D12ConvertFormat(desc.Format);
	props.mAllowRenderTarget = false;
	props.mAllowUnorderedAccess = false;
	props.mType = SR_ResourceType::Texture2D;
	props.mSourceFile = aTextureFilePath;
	props.mDebugName = aTextureFilePath.GetFileName();

	SC_Ref<SR_TextureResource_DX12> texres = SC_MakeRef<SR_TextureResource_DX12>(props, res);

	SC_Array<SR_PixelData> pixelData;

	uint32 i = 0;
	for (const D3D12_SUBRESOURCE_DATA& subres : subresourceData)
	{
		uint32 blockSize = SR_GetFormatBlockSize(props.mFormat);
		
		SR_PixelData& pd = pixelData.Add();
		pd.mBytesPerLine = (uint32)subres.RowPitch / blockSize;
		pd.mBytesPerSlice = (uint32)subres.SlicePitch / blockSize;
		pd.mData = (uint8*)subres.pData;
		pd.mLevel.mMipLevel = i;
		++i;
	}

	if (!texres->Init(pixelData.GetBuffer(), pixelData.Count()))
		return nullptr;


	SR_TextureProperties texProps(props.mFormat);

	switch (props.mType)
	{
	case SR_ResourceType::Texture1D:
		texProps.mDimension = SR_TextureDimension::Texture1D;
		break;
	case SR_ResourceType::Texture2D:
		texProps.mDimension = (props.mIsCubeMap) ? SR_TextureDimension::TextureCube : SR_TextureDimension::Texture2D;
		break;
	case SR_ResourceType::Texture3D:
		texProps.mDimension = SR_TextureDimension::Texture3D;
		break;
	default:
		assert(false);
	}

	return CreateTexture(texProps, texres);
#else
	SC_Ref<SR_Texture> texture;

	bool useCache = false;
	if (useCache)
	{
	}
	else
	{
		texture = LoadTextureInternal(aTextureFilePath);
	}

	return texture;
#endif
}

SC_Ref<SR_Texture> SR_RenderDevice_DX12::LoadTextureFromFile(const char* aTextureFilePath)
{
	SR_TextureData textureData;
	if (!SR_LoadTextureData(aTextureFilePath, textureData))
	{
		return nullptr;
	}

	const SR_TextureResourceProperties& props = textureData.mProperties;
	SC_Ref<SR_TextureResource> resource = CreateTextureResource(props, textureData.mPixelData.GetBuffer(), textureData.mPixelData.Count());

	SR_TextureProperties textureProperties(props.mFormat);
	return CreateTexture(textureProperties, resource);
}

#endif