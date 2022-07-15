
#include "SR_RenderDevice_DX12.h"

#if ENABLE_DX12

// D3D12 Agility SDK
extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 700; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = "./"; }
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
#include "SR_TempResourceHeap_DX12.h"
#include "SR_Fence_DX12.h"
#include "RenderCore/Resources/SR_TextureLoading.h"
#include "RenderCore/ShaderCompiler/SR_DirectXShaderCompiler.h"

#define USE_D3D12_DDS_LOADER (1)
#if USE_D3D12_DDS_LOADER
	#include "RenderCore/Resources/DDSTextureLoader.h"
#endif

#if IS_PC_PLATFORM
	#if ENABLE_NVAPI
		#include "nvapi.h"
	#endif
	#if ENABLE_AGS
		#include "amd_ags.h"
	#endif

	#if ENABLE_NVIDIA_AFTERMATH
		#include "GFSDK_Aftermath.h"
		#include "GFSDK_Aftermath_GpuCrashDump.h"
		#include "GFSDK_Aftermath_GpuCrashDumpDecoding.h"
	#endif
#endif

#if ENABLE_DIRECTSTORAGE
	#include "dstorage.h"
#endif

SR_RenderDevice_DX12* SR_RenderDevice_DX12::gInstance = nullptr;
bool SR_RenderDevice_DX12::gUsingNvApi = false;
bool SR_RenderDevice_DX12::gUsingAGS = false;

SR_RenderDevice_DX12::SR_RenderDevice_DX12()
	: SR_RenderDevice(SR_API::D3D12)
#if ENABLE_DRED
	, mEnableDRED(false)
#endif
{
	if (gInstance != nullptr)
		SC_ASSERT(false, "Already created RenderDevice.");
	else
		gInstance = this;

#if ENABLE_DRED
	if (SC_CommandLine::HasCommand("dred"))
		mEnableDRED = true;
#endif

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

SC_Ref<SR_FenceResource> SR_RenderDevice_DX12::CreateFenceResource()
{
	SC_Ref<SR_FenceResource_DX12> dx12Fence = SC_MakeRef<SR_FenceResource_DX12>();
	if (!dx12Fence->Init())
	{
		return nullptr;
	}

	return dx12Fence;
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
	return mBindlessDescriptorHeap;
}

SR_DescriptorHeap* SR_RenderDevice_DX12::GetSamplerDescriptorHeap() const
{
	return mSamplerDescriptorHeap;
}

SR_DescriptorHeap* SR_RenderDevice_DX12::GetRTVDescriptorHeap() const
{
	return mRTVDescriptorHeap;
}

SR_DescriptorHeap* SR_RenderDevice_DX12::GetDSVDescriptorHeap() const
{
	return mDSVDescriptorHeap;
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

#if ENABLE_DRED
const char* DREDGetBreadcrumbOp(const D3D12_AUTO_BREADCRUMB_OP& aOP)
{
	switch (aOP)
	{
	case D3D12_AUTO_BREADCRUMB_OP_SETMARKER: return "SetMarker";
	case D3D12_AUTO_BREADCRUMB_OP_BEGINEVENT: return "BeginEvent";
	case D3D12_AUTO_BREADCRUMB_OP_ENDEVENT: return "EndEvent";
	case D3D12_AUTO_BREADCRUMB_OP_DRAWINSTANCED: return "DrawInstanced";
	case D3D12_AUTO_BREADCRUMB_OP_DRAWINDEXEDINSTANCED: return "DrawIndexedInstanced";
	case D3D12_AUTO_BREADCRUMB_OP_EXECUTEINDIRECT: return "ExecuteIndirect";
	case D3D12_AUTO_BREADCRUMB_OP_DISPATCH: return "Dispatch";
	case D3D12_AUTO_BREADCRUMB_OP_COPYBUFFERREGION: return "CopyBufferRegion";
	case D3D12_AUTO_BREADCRUMB_OP_COPYTEXTUREREGION: return "CopyTextureRegion";
	case D3D12_AUTO_BREADCRUMB_OP_COPYRESOURCE: return "CopySource";
	case D3D12_AUTO_BREADCRUMB_OP_COPYTILES: return "CopyTiles";
	case D3D12_AUTO_BREADCRUMB_OP_RESOLVESUBRESOURCE: return "ResolveSubresource";
	case D3D12_AUTO_BREADCRUMB_OP_CLEARRENDERTARGETVIEW: return "ClearRTV";
	case D3D12_AUTO_BREADCRUMB_OP_CLEARUNORDEREDACCESSVIEW: return "ClearUAV";
	case D3D12_AUTO_BREADCRUMB_OP_CLEARDEPTHSTENCILVIEW: return "ClearDSV";
	case D3D12_AUTO_BREADCRUMB_OP_RESOURCEBARRIER: return "ResourceBarrier";
	case D3D12_AUTO_BREADCRUMB_OP_EXECUTEBUNDLE: return "ExecuteBundle";
	case D3D12_AUTO_BREADCRUMB_OP_PRESENT: return "Present";
	case D3D12_AUTO_BREADCRUMB_OP_RESOLVEQUERYDATA: return "ResolveQueryData";
	case D3D12_AUTO_BREADCRUMB_OP_BEGINSUBMISSION: return "BeginSubmission";
	case D3D12_AUTO_BREADCRUMB_OP_ENDSUBMISSION: return "EndSubmission";
	case D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME: return "DecodeFrame";
	case D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES: return "ProcessFrames";
	case D3D12_AUTO_BREADCRUMB_OP_ATOMICCOPYBUFFERUINT: return "AtomicCopyBufferUint";
	case D3D12_AUTO_BREADCRUMB_OP_ATOMICCOPYBUFFERUINT64: return "AtomicCopyBufferUint64";
	case D3D12_AUTO_BREADCRUMB_OP_RESOLVESUBRESOURCEREGION: return "ResolveSubresourceRegion";
	case D3D12_AUTO_BREADCRUMB_OP_WRITEBUFFERIMMEDIATE: return "WriteBufferImmediate";
	case D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME1: return "DecodeFrame1";
	case D3D12_AUTO_BREADCRUMB_OP_SETPROTECTEDRESOURCESESSION: return "SetProtectedResourceSession";
	case D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME2: return "DecodeFrame2";
	case D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES1: return "ProcessFrames1";
	case D3D12_AUTO_BREADCRUMB_OP_BUILDRAYTRACINGACCELERATIONSTRUCTURE: return "BuildRaytracingAccelerationStructure";
	case D3D12_AUTO_BREADCRUMB_OP_EMITRAYTRACINGACCELERATIONSTRUCTUREPOSTBUILDINFO: return "EmitRaytracingAccelerationStructurePostBuildInfo";
	case D3D12_AUTO_BREADCRUMB_OP_COPYRAYTRACINGACCELERATIONSTRUCTURE: return "CopyRaytracingAccelerationStructure";
	case D3D12_AUTO_BREADCRUMB_OP_DISPATCHRAYS: return "DispatchRays";
	case D3D12_AUTO_BREADCRUMB_OP_INITIALIZEMETACOMMAND: return "InitializeMetaCommand";
	case D3D12_AUTO_BREADCRUMB_OP_EXECUTEMETACOMMAND: return "ExecuteMetaCommand";
	case D3D12_AUTO_BREADCRUMB_OP_ESTIMATEMOTION: return "EstimateMotion";
	case D3D12_AUTO_BREADCRUMB_OP_RESOLVEMOTIONVECTORHEAP: return "ResolveMotionVectorHeap";
	case D3D12_AUTO_BREADCRUMB_OP_SETPIPELINESTATE1: return "SetPipelineState1";
	case D3D12_AUTO_BREADCRUMB_OP_INITIALIZEEXTENSIONCOMMAND: return "InitializeExtensionCommand";
	case D3D12_AUTO_BREADCRUMB_OP_EXECUTEEXTENSIONCOMMAND: return "ExecuteExtensionCommand";
	case D3D12_AUTO_BREADCRUMB_OP_DISPATCHMESH: return "DispatchMesh";
	case D3D12_AUTO_BREADCRUMB_OP_ENCODEFRAME: return "EncodeFrame";
	case D3D12_AUTO_BREADCRUMB_OP_RESOLVEENCODEROUTPUTMETADATA: return "ResolveEncoderOutputMetaData";
	default: return "<Unknown>";
	}
}

static const char* DREDGetAllocationType(const D3D12_DRED_ALLOCATION_TYPE& aType)
{
	switch (aType)
	{
	case D3D12_DRED_ALLOCATION_TYPE_COMMAND_QUEUE: return "CommandQueue";
	case D3D12_DRED_ALLOCATION_TYPE_COMMAND_ALLOCATOR: return "CommandAllocator";
	case D3D12_DRED_ALLOCATION_TYPE_PIPELINE_STATE: return "PipelineState";
	case D3D12_DRED_ALLOCATION_TYPE_COMMAND_LIST: return "CommandList";
	case D3D12_DRED_ALLOCATION_TYPE_FENCE: return "Fence";
	case D3D12_DRED_ALLOCATION_TYPE_DESCRIPTOR_HEAP: return "DescriptorHeap";
	case D3D12_DRED_ALLOCATION_TYPE_HEAP: return "Heap";
	case D3D12_DRED_ALLOCATION_TYPE_QUERY_HEAP: return "QueryHeap";
	case D3D12_DRED_ALLOCATION_TYPE_COMMAND_SIGNATURE: return "CommandSignature";
	case D3D12_DRED_ALLOCATION_TYPE_PIPELINE_LIBRARY: return "PipelineLibrary";
	case D3D12_DRED_ALLOCATION_TYPE_VIDEO_DECODER: return "VideoDecoder";
	case D3D12_DRED_ALLOCATION_TYPE_VIDEO_PROCESSOR: return "VideoProcessor";
	case D3D12_DRED_ALLOCATION_TYPE_RESOURCE: return "Resource";
	case D3D12_DRED_ALLOCATION_TYPE_PASS: return "Pass";
	case D3D12_DRED_ALLOCATION_TYPE_CRYPTOSESSION: return "CryptoSession";
	case D3D12_DRED_ALLOCATION_TYPE_CRYPTOSESSIONPOLICY: return "CryptoSessionPolicy";
	case D3D12_DRED_ALLOCATION_TYPE_PROTECTEDRESOURCESESSION: return "ProtectedResourceSession";
	case D3D12_DRED_ALLOCATION_TYPE_VIDEO_DECODER_HEAP: return "VideoDecoderHeap";
	case D3D12_DRED_ALLOCATION_TYPE_COMMAND_POOL: return "CommandPool";
	case D3D12_DRED_ALLOCATION_TYPE_COMMAND_RECORDER: return "CommandRecorder";
	case D3D12_DRED_ALLOCATION_TYPE_STATE_OBJECT: return "StateObject";
	case D3D12_DRED_ALLOCATION_TYPE_METACOMMAND: return "MetaCommand";
	case D3D12_DRED_ALLOCATION_TYPE_SCHEDULINGGROUP: return "SchedulingGroup";
	case D3D12_DRED_ALLOCATION_TYPE_VIDEO_MOTION_ESTIMATOR: return "VideoMotionEstimator";
	case D3D12_DRED_ALLOCATION_TYPE_VIDEO_MOTION_VECTOR_HEAP: return "VideoMotionVectorHeap";
	case D3D12_DRED_ALLOCATION_TYPE_VIDEO_EXTENSION_COMMAND: return "VideoExtensionCommand";
	case D3D12_DRED_ALLOCATION_TYPE_VIDEO_ENCODER: return "VideoEncoder";
	case D3D12_DRED_ALLOCATION_TYPE_VIDEO_ENCODER_HEAP: return "VideoEncoderHeap";
	default: return "<Unknown>";
	};
}

void SR_RenderDevice_DX12::OutputDredDebugData()
{
	if (!mEnableDRED)
		return;

	SC_MutexLock lock(mDREDMutex);

	static constexpr uint32 AutoBreadcrumbsBufferSizeInBytes = 65536;
	static constexpr uint32 AutoBreadcrumbsCommandHistoryOffset = 4096;
	static constexpr uint32 AutoBreadcrumbsCommandHistoryMax = (AutoBreadcrumbsBufferSizeInBytes - AutoBreadcrumbsCommandHistoryOffset) / 4;

	SR_ComPtr<ID3D12DeviceRemovedExtendedData1> dred;
	if(FAILED(mD3D12Device->QueryInterface(IID_PPV_ARGS(&dred)))) 
		return;

	D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT1 dredAutoBreadcrumbsOutput;
	if (FAILED(dred->GetAutoBreadcrumbsOutput1(&dredAutoBreadcrumbsOutput))) 
		return;

	D3D12_DRED_PAGE_FAULT_OUTPUT dredPageFaultOutput;
	if (FAILED(dred->GetPageFaultAllocationOutput(&dredPageFaultOutput))) 
		return;

	SC_ERROR("<----- DRED DEBUG OUTPUT BEGIN ----->\n");
	SC_ERROR("GPU Page Fault: VA: {:0x}", dredPageFaultOutput.PageFaultVA);
	SC_ERROR("<----- RECENTLY FREED RESOURCES BEGIN ----->");
	const D3D12_DRED_ALLOCATION_NODE* resourcePage = dredPageFaultOutput.pHeadRecentFreedAllocationNode;
	while (resourcePage)
	{
		SC_ERROR("\t>>[Type: {}] - {}", DREDGetAllocationType(resourcePage->AllocationType), SC_UTF16ToUTF8(resourcePage->ObjectNameW).c_str());
		resourcePage = resourcePage->pNext;
	}
	SC_ERROR("<----- RECENTLY FREED RESOURCES BEGIN ----->");

	SC_ERROR("<----- BREADCRUMBS BEGIN ----->");
	uint32 nodeIdx = 0;
	const D3D12_AUTO_BREADCRUMB_NODE1* node = dredAutoBreadcrumbsOutput.pHeadAutoBreadcrumbNode;
	while (node)
	{
		SC_ERROR("\t---- BREADCRUMB NODE {} BEGIN ----", nodeIdx);

		const uint32 completedOp = *node->pLastBreadcrumbValue;

		int32 contextIndex = 0;
		while (contextIndex < (int32)node->BreadcrumbContextsCount && node->pBreadcrumbContexts[contextIndex].BreadcrumbIndex < completedOp)
			++contextIndex;
		--contextIndex;

		uint32 count = 0;
		while (count < completedOp && count < AutoBreadcrumbsCommandHistoryMax)
		{
			std::string contextString("None");
			uint32 index = completedOp - count - 1;
			if (contextIndex >= 0 && node->pBreadcrumbContexts[contextIndex].BreadcrumbIndex == index)
			{
				contextString = SC_UTF16ToUTF8(node->pBreadcrumbContexts[contextIndex].pContextString);
				--contextIndex;
			}

			++count;
			SC_ERROR("\t\t>>[Context: {}] - Command: {}", contextString.c_str(), DREDGetBreadcrumbOp(node->pCommandHistory[index]));
		}

		SC_ERROR("---- BREADCRUMB NODE {} END ----", nodeIdx);
		++nodeIdx;
		node = node->pNext;
	}
	SC_ERROR("<----- BREADCRUMBS END ----->");
	SC_ERROR("<----- DRED DEBUG OUTPUT END ----->\n");
}
#endif //ENABLE_DRED

SC_SizeT SR_RenderDevice_DX12::GetAvailableVRAM() const
{
	DXGI_ADAPTER_DESC1 adapterDesc = {};
	mDXGIAdapter->GetDesc1(&adapterDesc);
	return adapterDesc.DedicatedVideoMemory;
}
SC_SizeT SR_RenderDevice_DX12::GetUsedVRAM() const
{
	DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo = {};
	mDXGIAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo);
	return videoMemoryInfo.CurrentUsage;
}

bool SR_RenderDevice_DX12::Init(void* /*aWindowHandle*/)
{
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&mDXGIFactory));

	SR_ComPtr<IDXGIAdapter1> tempAdapter;
	DXGI_ADAPTER_DESC1 adapterDesc = {};
	for (uint32 i = 0; mDXGIFactory->EnumAdapters1(i, &tempAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC1 desc = {};
		tempAdapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		if (desc.DedicatedVideoMemory > adapterDesc.DedicatedVideoMemory)
		{
			adapterDesc = desc;
			mDXGIAdapter = tempAdapter;
		}
	}

	{
		if (adapterDesc.VendorId == 0x10DE)
		{
			mSupportCaps.mDeviceInfo.mVendor = SR_GpuVendor::Nvidia;
			SC_LOG("Graphics Vendor: Nvidia");
		}
		else if (adapterDesc.VendorId == 0x1002)
		{
			mSupportCaps.mDeviceInfo.mVendor = SR_GpuVendor::AMD;
			SC_LOG("Graphics Vendor: AMD");
		}
		else if (adapterDesc.VendorId == 0x163C || adapterDesc.VendorId == 0x8086)
		{
			mSupportCaps.mDeviceInfo.mVendor = SR_GpuVendor::Intel;
			SC_LOG("Graphics Vendor: Intel");
		}

		mSupportCaps.mDeviceInfo.mDeviceName = SC_UTF16ToUTF8(adapterDesc.Description);
		mSupportCaps.mDeviceInfo.mDedicatedVRAM = uint32(adapterDesc.DedicatedVideoMemory >> 20);

		SC_LOG("Graphics card: {0} (id: {1} rev: {2})", mSupportCaps.mDeviceInfo.mDeviceName.c_str(), adapterDesc.DeviceId, adapterDesc.Revision);
		SC_LOG("Video Memory: {0}MB", mSupportCaps.mDeviceInfo.mDedicatedVRAM);
	}

	if (mEnableDebugMode)
	{
		SC_LOG("D3D12 Debug Layer: Enabled");
		SR_ComPtr<ID3D12Debug> debugger;
		D3D12GetDebugInterface(IID_PPV_ARGS(&debugger));
		debugger->EnableDebugLayer();
	}

#if ENABLE_DRED
	if (mEnableDRED)
	{
		SC_LOG("D3D12 DRED: Enabled");
		SR_ComPtr<ID3D12DeviceRemovedExtendedDataSettings1> dredSettings;
		D3D12GetDebugInterface(IID_PPV_ARGS(&dredSettings));
		dredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		dredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
	}
#endif

	hr = mDXGIAdapter.As(&mDXGIAdapter3);

	SC_LOG("Creating D3D12Device.");
	hr = D3D12CreateDevice(mDXGIAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mD3D12Device));


#if IS_PC_PLATFORM
#if ENABLE_NVIDIA_AFTERMATH
	if (SC_CommandLine::HasCommand("enableaftermath"))
	{
		const uint32 aftermathFlags =
			GFSDK_Aftermath_FeatureFlags_EnableMarkers |           // Enable event marker tracking.
			GFSDK_Aftermath_FeatureFlags_EnableResourceTracking |  // Enable tracking of resources.
			GFSDK_Aftermath_FeatureFlags_CallStackCapturing;// |      // Capture call stacks for all draw calls, compute dispatches, and resource copies.
			//GFSDK_Aftermath_FeatureFlags_GenerateShaderDebugInfo;  // Generate debug information for shaders.

		GFSDK_Aftermath_DX12_Initialize(GFSDK_Aftermath_Version_API, aftermathFlags, mD3D12Device.Get());

		if (GFSDK_Aftermath_EnableGpuCrashDumps(
			GFSDK_Aftermath_Version_API,
			GFSDK_Aftermath_GpuCrashDumpWatchedApiFlags_DX,
			GFSDK_Aftermath_GpuCrashDumpFeatureFlags_DeferDebugInfoCallbacks,
			GpuCrashDumpCallback,												// Register callback for GPU crash dumps.
			nullptr,															// Register callback for shader debug information. (Optional)
			nullptr,															// Register callback for GPU crash dump description. (Optional)
			this)																// Set the GpuCrashTracker object as user data for the above callbacks.
			!= GFSDK_Aftermath_Result_Success)
			return false;
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
	if (!mCommandQueues[static_cast<uint32>(SR_CommandListType::Graphics)]->Init(SR_CommandListType::Graphics, SR_CommandQueue::GetTypeName(SR_CommandListType::Graphics)))
		return false;

	mCommandQueues[static_cast<uint32>(SR_CommandListType::Compute)] = SC_MakeUnique<SR_CommandQueue_DX12>(this);
	if (!mCommandQueues[static_cast<uint32>(SR_CommandListType::Compute)]->Init(SR_CommandListType::Compute, SR_CommandQueue::GetTypeName(SR_CommandListType::Compute)))
		return false;

	mCommandQueues[static_cast<uint32>(SR_CommandListType::Copy)] = SC_MakeUnique<SR_CommandQueue_DX12>(this);
	if (!mCommandQueues[static_cast<uint32>(SR_CommandListType::Copy)]->Init(SR_CommandListType::Copy, SR_CommandQueue::GetTypeName(SR_CommandListType::Copy)))
		return false;

	// Create Descriptor heaps
	mBindlessDescriptorHeap = SC_MakeRef<SR_DescriptorHeap_DX12>(8192, SR_DescriptorHeapType::CBV_SRV_UAV, true);
	mSamplerDescriptorHeap = SC_MakeRef<SR_DescriptorHeap_DX12>(128, SR_DescriptorHeapType::SAMPLER, true);
	mRTVDescriptorHeap = SC_MakeRef<SR_DescriptorHeap_DX12>(512, SR_DescriptorHeapType::RTV);
	mDSVDescriptorHeap = SC_MakeRef<SR_DescriptorHeap_DX12>(32, SR_DescriptorHeapType::DSV);

	CreateDefaultRootSignatures(); 

	mDxcCompiler = SC_MakeUnique<SR_DirectXShaderCompiler>();

	mTempResourceHeap = SC_MakeUnique<SR_TempResourceHeap_DX12>();
	if (!mTempResourceHeap->Init())
		return false;

#if ENABLE_DIRECTSTORAGE
	SR_ComPtr<IDStorageFactory> storageFactory;
	DStorageGetFactory(IID_PPV_ARGS(&storageFactory));

	DSTORAGE_QUEUE_DESC queueDesc = {};
	queueDesc.Capacity = DSTORAGE_MAX_QUEUE_CAPACITY;
	queueDesc.Priority = DSTORAGE_PRIORITY_NORMAL;
	queueDesc.SourceType = DSTORAGE_REQUEST_SOURCE_FILE;
	queueDesc.Device = mD3D12Device.Get();

	SR_ComPtr<IDStorageQueue> storageQueue;
	storageFactory->CreateQueue(&queueDesc, IID_PPV_ARGS(&storageQueue));
#endif

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
#if IS_PC_PLATFORM
#if ENABLE_NVAPI
	if (mSupportCaps.mDeviceInfo.mVendor == SR_GpuVendor::Nvidia && !SC_CommandLine::HasCommand("nonvapi"))
	{
		if (NvAPI_Initialize() == NVAPI_OK)
		{
			gUsingNvApi = true;

			NvU32 driverVersion;
			NvAPI_ShortString driverString;
			if (NvAPI_SYS_GetDriverAndBranchVersion(&driverVersion, driverString) == NVAPI_OK)
			{
				mSupportCaps.mDeviceInfo.mDriverVersion = driverVersion;
				mSupportCaps.mDeviceInfo.mDriverVersionString = driverString;
				SC_LOG("NvAPI: Driver: {} (Driver String: {})", driverVersion, driverString);
			}

			DXGI_ADAPTER_DESC1 adapterDesc = {};
			mDXGIAdapter->GetDesc1(&adapterDesc);

			NvPhysicalGpuHandle nvGPUHandles[NVAPI_MAX_PHYSICAL_GPUS];
			NvU32 numGPUs = 0;
			if (NvAPI_EnumPhysicalGPUs(nvGPUHandles, &numGPUs) == NVAPI_OK)
			{
				for (uint32 i = 0; i < numGPUs; ++i)
				{
					NvPhysicalGpuHandle& gpuHandle = nvGPUHandles[i];
					NvU32 deviceId = 0;
					NvU32 subSystemId = 0;
					NvU32 revisionId = 0;
					NvU32 extDeviceId = 0;
					if (NvAPI_GPU_GetPCIIdentifiers(gpuHandle, &deviceId, &subSystemId, &revisionId, &extDeviceId) == NVAPI_OK)
					{
						if (extDeviceId == adapterDesc.DeviceId && revisionId == adapterDesc.Revision && subSystemId == adapterDesc.SubSysId)
						{
							NvAPI_ShortString fullName;
							if (NvAPI_GPU_GetFullName(gpuHandle, fullName) == NVAPI_OK)
								SC_LOG("NvAPI: {}", fullName);

							NvU32 coreCount = 0;
							if (NvAPI_GPU_GetGpuCoreCount(gpuHandle, &coreCount) == NVAPI_OK)
							{
								SC_LOG("NvAPI: Num Shader Cores: {}", coreCount);
								mSupportCaps.mDeviceInfo.mNumShaderCores = coreCount;
							}

							NV_GPU_ARCH_INFO architectureInfo;
							architectureInfo.version = NV_GPU_ARCH_INFO_VER;
							if (NvAPI_GPU_GetArchInfo(gpuHandle, &architectureInfo) == NVAPI_OK)
							{
								switch (architectureInfo.architecture_id)
								{
								case NV_GPU_ARCHITECTURE_GK100:
								case NV_GPU_ARCHITECTURE_GK110:
								case NV_GPU_ARCHITECTURE_GK200:
									mSupportCaps.mDeviceInfo.mArchitecture = SR_GpuArchitecture::Nv_Kepler;
									SC_LOG("NvAPI: Gpu Architecture: Kepler");
									break;
								case NV_GPU_ARCHITECTURE_GM000:
								case NV_GPU_ARCHITECTURE_GM200:
									mSupportCaps.mDeviceInfo.mArchitecture = SR_GpuArchitecture::Nv_Maxwell;
									SC_LOG("NvAPI: Gpu Architecture: Maxwell");
									break;
								case NV_GPU_ARCHITECTURE_GP100:
									mSupportCaps.mDeviceInfo.mArchitecture = SR_GpuArchitecture::Nv_Pascal;
									SC_LOG("NvAPI: Gpu Architecture: Pascal");
									break;
								case NV_GPU_ARCHITECTURE_TU100:
									mSupportCaps.mDeviceInfo.mArchitecture = SR_GpuArchitecture::Nv_Turing;
									SC_LOG("NvAPI: Gpu Architecture: Turing");
									break;
								case NV_GPU_ARCHITECTURE_GV100:
								case NV_GPU_ARCHITECTURE_GV110:
									mSupportCaps.mDeviceInfo.mArchitecture = SR_GpuArchitecture::Nv_Volta;
									SC_LOG("NvAPI: Gpu Architecture: Volta");
									break;
								case NV_GPU_ARCHITECTURE_GA100:
									mSupportCaps.mDeviceInfo.mArchitecture = SR_GpuArchitecture::Nv_Ampere;
									SC_LOG("NvAPI: Gpu Architecture: Ampere");
									break;
								default:
									break;
								}
							}

							break;
						}
					}
				}
			}
		}
	}
#endif //ENABLE_NVAPI
#if ENABLE_AGS
	else if (mSupportCaps.mDeviceInfo.mVendor == SR_GpuVendor::AMD && !SC_CommandLine::HasCommand("noags"))
	{
		AGSConfiguration config = {};
		AGSGPUInfo gpuInfo = {};
		AGSReturnCode status = agsInitialize(AGS_MAKE_VERSION(AMD_AGS_VERSION_MAJOR, AMD_AGS_VERSION_MINOR, AMD_AGS_VERSION_PATCH), &config, nullptr, &gpuInfo);
		if (status == AGS_SUCCESS)
		{
			gUsingAGS = true;

		}
	}
#endif //ENABLE_AGS
#endif //IS_DESKTOP_PLATFORM 

	mSupportCaps.mEnableAsyncCompute = true;
	if (SC_CommandLine::HasCommand("noasynccompute"))
		mSupportCaps.mEnableAsyncCompute = false;
	SC_LOG("Asynchronous Compute: {}", mSupportCaps.mEnableAsyncCompute ? "true" : "false");

	D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {};
	shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_7;
	HRESULT hr = mD3D12Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
	while (hr == E_INVALIDARG && shaderModel.HighestShaderModel > D3D_SHADER_MODEL_6_0)
	{
		shaderModel.HighestShaderModel = D3D_SHADER_MODEL(shaderModel.HighestShaderModel - 1);
		hr = mD3D12Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
	}
	mSupportCaps.mHighestShaderModel = SR_D3D12ConvertShaderModel(shaderModel.HighestShaderModel);
	SC_LOG("Shader Model: {}", GetShaderModelString(mSupportCaps.mHighestShaderModel));

	D3D12_FEATURE_DATA_D3D12_OPTIONS dx12Options = {};
	hr = mD3D12Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &dx12Options, sizeof(dx12Options));
	if (!VerifyHRESULT(hr))
	{
		SC_ERROR("Could not check options from device");
		return;
	}

	mSupportCaps.mEnableConservativeRasterization = dx12Options.ConservativeRasterizationTier > D3D12_CONSERVATIVE_RASTERIZATION_TIER_NOT_SUPPORTED;
	SC_LOG("Conservative Rasterization: {}", mSupportCaps.mEnableConservativeRasterization ? "true" : "false");

	D3D12_FEATURE_DATA_D3D12_OPTIONS5 dx12Options5 = {};
	hr = mD3D12Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &dx12Options5, sizeof(dx12Options5));
	if (!VerifyHRESULT(hr))
	{
		SC_ERROR("Could not check Options5 from device");
		return;
	}

	switch (dx12Options5.RaytracingTier)
	{
	case D3D12_RAYTRACING_TIER_1_0:
		mSupportCaps.mRaytracingType = SR_RaytracingType::Default;
		SC_LOG("Raytracing: HW");
		break;
	case D3D12_RAYTRACING_TIER_1_1:
		mSupportCaps.mRaytracingType = SR_RaytracingType::Inline;
		SC_LOG("Raytracing: HW Inline");
		break;
	default:
		SC_LOG("Raytracing: None");
		break;
	}

	D3D12_FEATURE_DATA_D3D12_OPTIONS6 dx12Options6 = {};
	hr = mD3D12Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &dx12Options6, sizeof(dx12Options6));
	if (!VerifyHRESULT(hr))
	{
		SC_ERROR("Could not check Options6 from device");
		return;
	}

	mSupportCaps.mEnableVRS = dx12Options6.VariableShadingRateTier > D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED;
	SC_LOG("Variable Rate Shading: {}", mSupportCaps.mEnableVRS ? "true" : "false");

	D3D12_FEATURE_DATA_D3D12_OPTIONS7 dx12Options7 = {};
	hr = mD3D12Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &dx12Options7, sizeof(dx12Options7));
	if (!VerifyHRESULT(hr))
	{
		SC_ERROR("Could not check Options7 from device");
		return;
	}

	mSupportCaps.mEnableMeshShaders = dx12Options7.MeshShaderTier > D3D12_MESH_SHADER_TIER_NOT_SUPPORTED;
	mSupportCaps.mEnableSamplerFeedback = dx12Options7.SamplerFeedbackTier > D3D12_SAMPLER_FEEDBACK_TIER_NOT_SUPPORTED;
	//float meshShadersTier = dx12Options7.MeshShaderTier / 10.f;
	SC_LOG("Mesh Shaders: {}", mSupportCaps.mEnableMeshShaders ? "true" : "false");
	SC_LOG("Sampler Feedback: {}", mSupportCaps.mEnableSamplerFeedback ? "true" : "false");
}

#if ENABLE_NVIDIA_AFTERMATH
void SR_RenderDevice_DX12::GpuCrashDumpCallback(const void* aGpuCrashDump, const uint32 aGpuCrashDumpSize, void* aUserData)
{
	SR_RenderDevice_DX12* renderDevice = reinterpret_cast<SR_RenderDevice_DX12*>(aUserData);
	renderDevice->OnGpuCrashDump(aGpuCrashDump, aGpuCrashDumpSize);
}
void SR_RenderDevice_DX12::OnGpuCrashDump(const void* aGpuCrashDump, const uint32 aGpuCrashDumpSize)
{
	// Create a GPU crash dump decoder object for the GPU crash dump.
	GFSDK_Aftermath_GpuCrashDump_Decoder decoder = {};
	GFSDK_Aftermath_GpuCrashDump_CreateDecoder(
		GFSDK_Aftermath_Version_API,
		aGpuCrashDump,
		aGpuCrashDumpSize,
		&decoder);

	// Use the decoder object to read basic information, like application
	// name, PID, etc. from the GPU crash dump.
	GFSDK_Aftermath_GpuCrashDump_BaseInfo baseInfo = {};
	GFSDK_Aftermath_GpuCrashDump_GetBaseInfo(decoder, &baseInfo);

	// Use the decoder object to query the application name that was set
	// in the GPU crash dump description.
	uint32 applicationNameLength = 0;
	GFSDK_Aftermath_GpuCrashDump_GetDescriptionSize(
		decoder,
		GFSDK_Aftermath_GpuCrashDumpDescriptionKey_ApplicationName,
		&applicationNameLength);

	std::vector<char> applicationName(applicationNameLength, '\0');

	GFSDK_Aftermath_GpuCrashDump_GetDescription(
		decoder,
		GFSDK_Aftermath_GpuCrashDumpDescriptionKey_ApplicationName,
		uint32(applicationName.size()),
		applicationName.data());

	// Create a unique file name for writing the crash dump data to a file.
	// Note: due to an Nsight Aftermath bug (will be fixed in an upcoming
	// driver release) we may see redundant crash dumps. As a workaround,
	// attach a unique count to each generated file name.
	static int count = 0;
	const std::string baseFileName =
		std::string(applicationName.data())
		+ "-"
		+ std::to_string(baseInfo.pid)
		+ "-"
		+ std::to_string(++count);

	// Write the the crash dump data to a file using the .nv-gpudmp extension
	// registered with Nsight Graphics.
	const std::string crashDumpFileName = baseFileName + ".nv-gpudmp";
	std::ofstream dumpFile(crashDumpFileName, std::ios::out | std::ios::binary);
	if (dumpFile)
	{
		dumpFile.write((const char*)aGpuCrashDump, aGpuCrashDumpSize);
		dumpFile.close();
	}

	// Decode the crash dump to a JSON string.
	// Step 1: Generate the JSON and get the size.
	uint32 jsonSize = 0;
	GFSDK_Aftermath_GpuCrashDump_GenerateJSON(
		decoder,
		GFSDK_Aftermath_GpuCrashDumpDecoderFlags_ALL_INFO,
		GFSDK_Aftermath_GpuCrashDumpFormatterFlags_NONE,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		this,
		&jsonSize);
	// Step 2: Allocate a buffer and fetch the generated JSON.
	std::vector<char> json(jsonSize);
	GFSDK_Aftermath_GpuCrashDump_GetJSON(
		decoder,
		uint32(json.size()),
		json.data());

	// Write the the crash dump data as JSON to a file.
	const std::string jsonFileName = crashDumpFileName + ".json";
	std::ofstream jsonFile(jsonFileName, std::ios::out | std::ios::binary);
	if (jsonFile)
	{
		jsonFile.write(json.data(), json.size());
		jsonFile.close();
	}

	// Destroy the GPU crash dump decoder object.
	GFSDK_Aftermath_GpuCrashDump_DestroyDecoder(decoder);
}
#endif

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
		SC_ASSERT(false);
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