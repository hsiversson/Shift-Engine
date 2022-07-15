#include "SR_RenderDevice.h"

#include "RenderCore/RenderTasks/SR_RenderThread.h"

#if SR_ENABLE_DX12
#include "RenderCore/DX12/SR_RenderDevice_DX12.h"
#endif

#if SR_ENABLE_RENDERDOC_API
#include "renderdoc_app.h"
#endif

class SR_TextureCache
{
public:
	static SR_TextureCache* Get()
	{
		if (!gInstance)
			gInstance = new SR_TextureCache;

		return gInstance;
	}

	SC_Ref<SR_Texture> GetResource(const SC_FilePath& aSourceFile)
	{
		std::string f(aSourceFile.GetStr());

		if (mCache.find(f) != mCache.end())
			return mCache[f];

		return nullptr;
	}

	void Insert(const SC_FilePath& aSourceFile, SC_Ref<SR_Texture> aTexture)
	{
		std::string f(aSourceFile.GetStr());
		mCache[f] = aTexture;
	}

	void RemoveIfLastReference(const SC_FilePath& aSourceFile)
	{
		std::string f(aSourceFile.GetStr());
		if (mCache.find(f) != mCache.end())
			return;

		if (mCache[f].GetRefCount() <= 2) // The cache holds one ref
		{
			mCache.erase(f);
		}
	}

private:
	static SR_TextureCache* gInstance;

	SC_UnorderedMap<std::string, SC_Ref<SR_Texture>> mCache;

};
SR_TextureCache* SR_TextureCache::gInstance = nullptr;


SR_RenderDevice* SR_RenderDevice::gInstance = nullptr;

SR_RenderDevice::~SR_RenderDevice()
{

}

void SR_RenderDevice::Present()
{
	uint32 frameIdx = SC_Time::gFrameCounter;
	auto presentTask = [&, frameIdx]()
	{
		SR_SwapChain* sc = GetSwapChain();
		sc->Present();
		mTempResourceHeap->EndFrame();
		SR_RenderThread::Get()->EndFrame(frameIdx);
	};
	SR_RenderThread::Get()->PostTask(presentTask);
}

SC_Ref<SR_CommandList> SR_RenderDevice::CreateCommandList(const SR_CommandListType& /*aType*/)
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

SC_Ref<SR_Texture> SR_RenderDevice::CreateTexture(const SR_TextureProperties& /*aTextureProperties*/, const SC_Ref<SR_TextureResource>& /*aResource*/)
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

SC_Ref<SR_RenderTarget> SR_RenderDevice::CreateRenderTarget(const SR_RenderTargetProperties& /*aRenderTargetProperties*/, const SC_Ref<SR_TextureResource>& /*aResource*/)
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

SC_Ref<SR_DepthStencil> SR_RenderDevice::CreateDepthStencil(const SR_DepthStencilProperties& /*aDepthStencilProperties*/, const SC_Ref<SR_TextureResource>& /*aResource*/)
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

SC_Ref<SR_TextureResource> SR_RenderDevice::CreateTextureResource(const SR_TextureResourceProperties& /*aTextureResourceProperties*/, const SR_PixelData* /*aInitialData = nullptr*/, uint32 /*aDataCount = 0*/)
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

SR_TempTexture SR_RenderDevice::CreateTempTexture(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture, bool aIsRenderTarget, bool aIsWritable)
{
	SC_ASSERT(mTempResourceHeap);
	return mTempResourceHeap->GetTexture(aTextureProperties, aIsTexture, aIsRenderTarget, aIsWritable);
}

SC_Ref<SR_Texture> SR_RenderDevice::LoadTexture(const SC_FilePath& aTextureFilePath)
{
	if (SC_Ref<SR_Texture> tex = SR_TextureCache::Get()->GetResource(aTextureFilePath))
	{
		return tex;
	}

	SC_Ref<SR_Texture> texture = LoadTextureInternal(aTextureFilePath);
	SR_TextureCache::Get()->Insert(aTextureFilePath, texture);
	return texture;
}

SC_Ref<SR_Buffer> SR_RenderDevice::CreateBuffer(const SR_BufferProperties& /*aBufferProperties*/, const SC_Ref<SR_BufferResource>& /*aResource*/)
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

SC_Ref<SR_BufferResource> SR_RenderDevice::CreateBufferResource(const SR_BufferResourceProperties& /*aBufferResourceProperties*/, const void* /*aInitialData*/)
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

SR_TempBuffer SR_RenderDevice::CreateTempBuffer(const SR_BufferResourceProperties& aBufferResourceProperties, bool aIsWritable)
{
	SC_ASSERT(mTempResourceHeap);
	return mTempResourceHeap->GetBuffer(aBufferResourceProperties, aIsWritable);
}

SC_Ref<SR_Heap> SR_RenderDevice::CreateHeap(const SR_HeapProperties& /*aHeapProperties*/)
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

SC_Ref<SR_FenceResource> SR_RenderDevice::CreateFenceResource()
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

bool SR_RenderDevice::CompileShader(const SR_ShaderCompileArgs& /*aArgs*/, SR_ShaderByteCode& /*aOutByteCode*/, SR_ShaderMetaData* /*aOutMetaData*/)
{
	SC_ASSERT(false, "Not implemented yet!");
	return false;
}

bool SR_RenderDevice::CompileShader(const std::string& /*aShadercode*/, const SR_ShaderCompileArgs& /*aArgs*/, SR_ShaderByteCode& /*aOutByteCode*/, SR_ShaderMetaData* /*aOutMetaData*/)
{
	SC_ASSERT(false, "Not implemented yet!");
	return false;
}

SC_Ref<SR_ShaderState> SR_RenderDevice::CreateShaderState(const SR_ShaderStateProperties& /*aProperties*/)
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

SC_Ref<SR_SwapChain> SR_RenderDevice::CreateSwapChain(const SR_SwapChainProperties& /*aProperties*/, void* /*aNativeWindowHandle*/)
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

bool SR_RenderDevice::IsFencePending(SR_Fence& aFence) const
{
	if (aFence.mType == SR_CommandListType::Unknown)
		return false;

	return aFence.IsPending();
}

void SR_RenderDevice::WaitForFence(SR_Fence& aFence)
{
	if (aFence.mType == SR_CommandListType::Unknown)
		return;

	aFence.Wait(true);
}

SR_CommandQueue* SR_RenderDevice::GetGraphicsCommandQueue() const
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

SR_CommandQueue* SR_RenderDevice::GetCommandQueue(const SR_CommandListType& /*aType*/) const
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

SR_DescriptorHeap* SR_RenderDevice::GetDefaultDescriptorHeap() const
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

SR_DescriptorHeap* SR_RenderDevice::GetSamplerDescriptorHeap() const
{
	return nullptr;
}

SR_DescriptorHeap* SR_RenderDevice::GetRTVDescriptorHeap() const
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

SR_DescriptorHeap* SR_RenderDevice::GetDSVDescriptorHeap() const
{
	SC_ASSERT(false, "Not implemented yet!");
	return nullptr;
}

SR_RootSignature* SR_RenderDevice::GetRootSignature(const SR_RootSignatureType& aType) const
{
	return mRootSignatures[static_cast<uint32>(aType)];
}

void SR_RenderDevice::SetSwapChain(SR_SwapChain* aSwapChain)
{
	mDefaultSwapChain.Reset(aSwapChain);
}

SR_SwapChain* SR_RenderDevice::GetSwapChain() const
{
	return mDefaultSwapChain;
}

SR_InstanceBuffer* SR_RenderDevice::GetPersistentResourceInfo() const
{
	return mPersistentResourceInfo.get();
}

SR_QueueManager* SR_RenderDevice::GetQueueManager() const
{
	return mQueueManager.get();
}

SC_Ref<SR_CommandList> SR_RenderDevice::GetTaskCommandList()
{
	if (!mQueueManager)
		return nullptr;

	return mQueueManager->GetCommandList(mQueueManager->gCurrentTaskType);
}

const SR_RenderSupportCaps& SR_RenderDevice::GetSupportCaps() const
{
	return mSupportCaps;
}

#if SR_ENABLE_RENDERDOC_API
void SR_RenderDevice::StartRenderDocCapture()
{
	if (mRenderDocAPI)
	{
		mRenderDocAPI->StartFrameCapture(nullptr, nullptr);
	}
}

void SR_RenderDevice::EndRenderDocCapture()
{
	if (mRenderDocAPI)
	{
		mRenderDocAPI->EndFrameCapture(nullptr, nullptr);
	}
}
#endif

SC_SizeT SR_RenderDevice::GetAvailableVRAM() const
{
	SC_ASSERT(false, "Not implemented yet!");
	return 0;
}
SC_SizeT SR_RenderDevice::GetUsedVRAM() const
{
	SC_ASSERT(false, "Not implemented yet!");
	return 0;
}

const SR_API& SR_RenderDevice::GetAPI() const
{
	return mRenderAPIType;
}

bool SR_RenderDevice::Create(const SR_API& aAPIType)
{
	if (gInstance)
	{
		SC_ASSERT(false, "RenderDevice can only be created once!");
		return false;
	}

	switch (aAPIType)
	{
#if SR_ENABLE_DX12
	case SR_API::D3D12:
		gInstance = new SR_RenderDevice_DX12();
		break;
#endif
#if SR_ENABLE_VULKAN
	case SR_API::Vulkan:
		gInstance = new SR_RenderDevice_Vk();
		break;
#endif
	case SR_API::Unknown:
	default:
		SC_ASSERT(false, "Invalid render API.");
		return false;
	}

	if (!gInstance->Init(nullptr))
	{
		SC_ASSERT(false, "Couldn't initialize render api.");
		return false;
	}

	return true;
}

void SR_RenderDevice::Destroy()
{
	if (gInstance == nullptr)
		return;

	delete gInstance;
	gInstance = nullptr;
}

SR_RenderDevice::SR_RenderDevice(const SR_API& aAPI)
	: mLatestFinishedFrame(0)
	, mDefaultSwapChain(nullptr)
	, mEnableDebugMode(false)
	, mBreakOnError(false)
#if SR_ENABLE_RENDERDOC_API
	, mEnableRenderDocCaptures(false)
#endif
	, mRenderAPIType(aAPI)
{
	SC_ASSERT(gInstance == nullptr && "Only 1 render device may exist!");
	gInstance = this;

	if (SC_CommandLine::HasCommand("debugrender"))
	{
		mEnableDebugMode = true;
	}
	if (SC_CommandLine::HasCommand("debugrenderbreak"))
	{
		mEnableDebugMode = true;
		mBreakOnError = true;
	}

#if SR_ENABLE_RENDERDOC_API
	if (mEnableRenderDocCaptures)
	{
		if (HMODULE renderDocModule = GetModuleHandleA("renderdoc.dll"))
		{
			pRENDERDOC_GetAPI GetRenderDocAPI = (pRENDERDOC_GetAPI)GetProcAddress(renderDocModule, "RENDERDOC_GetAPI");
			int result = GetRenderDocAPI(eRENDERDOC_API_Version_1_4_2, (void**)&mRenderDocAPI);
			SC_ASSERT(result == 1);
		}
	}
	else
		mRenderDocAPI = nullptr;
#endif

	mRenderThread = SC_MakeUnique<SR_RenderThread>();
}

bool SR_RenderDevice::Init(void* /*aWindowHandle*/)
{
	return false;
}

SC_Ref<SR_Texture> SR_RenderDevice::LoadTextureInternal(const SC_FilePath& /*aTextureFilePath*/)
{
	return nullptr;
}

bool SR_RenderDevice::PostInit()
{
	mPersistentResourceInfo = SC_MakeUnique<SR_InstanceBuffer>();
	if (!mPersistentResourceInfo->Init(32768))
	{
		return false;
	}

	mQueueManager = SC_MakeUnique<SR_QueueManager>();
	if (!mQueueManager->Init())
		return false;

	return true;
}
