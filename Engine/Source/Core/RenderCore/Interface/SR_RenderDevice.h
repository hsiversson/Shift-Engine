#pragma once
#include "RenderCore/Defines/SR_RenderDefines.h"

#include "SR_Texture.h"
#include "SR_RenderTarget.h"
#include "SR_DepthStencil.h"
#include "SR_TextureResource.h"
#include "SR_Buffer.h"
#include "SR_BufferResource.h"
#include "SR_Heap.h"
#include "SR_DescriptorHeap.h"
#include "SR_SwapChain.h"
#include "SR_CommandQueue.h"
#include "SR_ShaderState.h"
#include "SR_RootSignature.h"
#include "SR_TempResourceHeap.h"
#include "RenderCore/Resources/SR_InstanceBuffer.h"
#include "RenderCore/RenderTasks/SR_QueueManager.h"

#define ENABLE_NVAPI			(1)
#define ENABLE_AGS				(1)

enum class SR_GpuVendor
{
	Unknown,
	Nvidia,
	AMD,
	Intel
};
enum class SR_GpuArchitecture
{
	Unknown,
	
	Nv_Kepler,
	Nv_Maxwell,
	Nv_Pascal,
	Nv_Turing,
	Nv_Volta = Nv_Turing,
	Nv_Ampere,
	Nv_Lovlace,
	Nv_Hopper = Nv_Lovlace,

	Amd_GCN1,
	Amd_GCN2,
	Amd_GCN3,
	Amd_GCN4,
	Amd_Vega,
	Amd_RDNA,
	Amd_RDNA2,
	Amd_RDNA3,

	Intel_Xe,
	Intel_Xe2,
	Intel_Xe3,
};

struct SR_GpuDeviceInfo
{
	SR_GpuDeviceInfo()
		: mDriverVersion(0)
		, mVendor(SR_GpuVendor::Unknown)
		, mArchitecture(SR_GpuArchitecture::Unknown)
		, mDedicatedVRAM(0)
		, mNumShaderCores(0)
		, mTheoreticalMaxPerformance(0.0f)
	{}

	std::string mDeviceName;
	std::string mDriverVersionString;
	uint32 mDriverVersion;

	SR_GpuVendor mVendor;
	SR_GpuArchitecture mArchitecture;

	uint32 mDedicatedVRAM; // In Megabytes
	uint32 mNumShaderCores;
	float mTheoreticalMaxPerformance; // In TFLOPS
};

struct SR_RenderSupportCaps
{
	SR_RenderSupportCaps() 
		: mHighestShaderModel(SR_ShaderModel::SM_5_1)
		, mRaytracingType(SR_RaytracingType::None)
		, mEnableMeshShaders(false)
		, mEnableAsyncCompute(false)
		, mEnableAsyncCopy(false)
		, mEnableVRS(false)
		, mEnableSamplerFeedback(false)
		, mEnableConservativeRasterization(false)
	{}

	SR_GpuDeviceInfo mDeviceInfo;
	SR_ShaderModel mHighestShaderModel;
	SR_RaytracingType mRaytracingType;
	bool mEnableMeshShaders : 1;
	bool mEnableAsyncCompute : 1;
	bool mEnableAsyncCopy : 1;
	bool mEnableVRS : 1;
	bool mEnableSamplerFeedback : 1;
	bool mEnableConservativeRasterization : 1;
};

enum class SR_TempHeapTypes
{
	Constants,
	Staging,
	ShaderBuffer,
	COUNT
};

class SR_RenderThread;
class SR_RenderDevice
{
public:
	virtual ~SR_RenderDevice();

	void Present();

	virtual SC_Ref<SR_CommandList> CreateCommandList(const SR_CommandListType& aType);

	virtual SC_Ref<SR_Texture> CreateTexture(const SR_TextureProperties& aTextureProperties, const SC_Ref<SR_TextureResource>& aResource);
	virtual SC_Ref<SR_RenderTarget> CreateRenderTarget(const SR_RenderTargetProperties& aRenderTargetProperties, const SC_Ref<SR_TextureResource>& aResource);
	virtual SC_Ref<SR_DepthStencil> CreateDepthStencil(const SR_DepthStencilProperties& aDepthStencilProperties, const SC_Ref<SR_TextureResource>& aResource);
	virtual SC_Ref<SR_TextureResource> CreateTextureResource(const SR_TextureResourceProperties& aTextureResourceProperties, const SR_PixelData* aInitialData = nullptr, uint32 aDataCount = 0);

	SR_TempTexture CreateTempTexture(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture = true, bool aIsRenderTarget = false, bool aIsWritable = false);

	SC_Ref<SR_Texture> LoadTexture(const SC_FilePath& aTextureFilePath);

	virtual SC_Ref<SR_Buffer> CreateBuffer(const SR_BufferProperties& aBufferProperties, const SC_Ref<SR_BufferResource>& aResource);
	virtual SC_Ref<SR_BufferResource> CreateBufferResource(const SR_BufferResourceProperties& aBufferResourceProperties, const void* aInitialData = nullptr);

	SR_TempBuffer CreateTempBuffer(const SR_BufferResourceProperties& aBufferResourceProperties, bool aIsWritable = true);

	virtual SC_Ref<SR_Heap> CreateHeap(const SR_HeapProperties& aHeapProperties);

	virtual SC_Ref<SR_FenceResource> CreateFenceResource();

	virtual bool CompileShader(const SR_ShaderCompileArgs& aArgs, SR_ShaderByteCode& aOutByteCode, SR_ShaderMetaData* aOutMetaData = nullptr);
	virtual bool CompileShader(const std::string& aShadercode, const SR_ShaderCompileArgs& aArgs, SR_ShaderByteCode& aOutByteCode, SR_ShaderMetaData* aOutMetaData = nullptr);
	virtual SC_Ref<SR_ShaderState> CreateShaderState(const SR_ShaderStateProperties& aProperties);
	
	virtual SC_Ref<SR_SwapChain> CreateSwapChain(const SR_SwapChainProperties& aProperties, void* aNativeWindowHandle);

	virtual bool IsFencePending(SR_Fence& aFence) const;
	void WaitForFence(SR_Fence& aFence);
	virtual SR_CommandQueue* GetGraphicsCommandQueue() const;
	virtual SR_CommandQueue* GetCommandQueue(const SR_CommandListType& aType) const;

	virtual SR_DescriptorHeap* GetDefaultDescriptorHeap() const;
	virtual SR_DescriptorHeap* GetSamplerDescriptorHeap() const;
	virtual SR_DescriptorHeap* GetRTVDescriptorHeap() const;
	virtual SR_DescriptorHeap* GetDSVDescriptorHeap() const;

	SR_RootSignature* GetRootSignature(const SR_RootSignatureType& aType) const;

	void SetSwapChain(SR_SwapChain* aSwapChain);
	SR_SwapChain* GetSwapChain() const;

	SR_InstanceBuffer* GetPersistentResourceInfo() const;

	SR_QueueManager* GetQueueManager() const;
	SC_Ref<SR_CommandList> GetTaskCommandList(); // Returns the command list assigned to the current render task. Should only be called inside render tasks.

	const SR_RenderSupportCaps& GetSupportCaps() const;

#if SR_ENABLE_RENDERDOC_API
	void StartRenderDocCapture();
	void EndRenderDocCapture();
#endif

	virtual SC_SizeT GetAvailableVRAM() const;
	virtual SC_SizeT GetUsedVRAM() const;

	const SR_API& GetAPI() const;

	static bool Create(const SR_API& aAPIType);
	static void Destroy();
	static SR_RenderDevice* gInstance;
	uint64 mLatestFinishedFrame;
protected:
	SR_RenderDevice(const SR_API& aAPI);
	virtual bool Init(void* aWindowHandle);
	virtual SC_Ref<SR_Texture> LoadTextureInternal(const SC_FilePath& aTextureFilePath);
	bool PostInit();

	SC_UniquePtr<SR_RenderThread> mRenderThread;

	SC_Ref<SR_RootSignature> mRootSignatures[static_cast<uint32>(SR_RootSignatureType::COUNT)];

	SC_Ref<SR_SwapChain> mDefaultSwapChain;

	SC_UniquePtr<SR_InstanceBuffer> mPersistentResourceInfo;

	SC_UniquePtr<SR_QueueManager> mQueueManager;

	SC_UniquePtr<SR_TempResourceHeap> mTempResourceHeap;

	SR_RenderSupportCaps mSupportCaps;
	bool mEnableDebugMode : 1;
	bool mBreakOnError : 1;

#if SR_ENABLE_RENDERDOC_API
	RENDERDOC_API_1_4_1* mRenderDocAPI;
	bool mEnableRenderDocCaptures;
#endif

	const SR_API mRenderAPIType;
};

