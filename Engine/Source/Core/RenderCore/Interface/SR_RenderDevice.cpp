#include "SR_RenderDevice.h"

#if ENABLE_DX12
#include "RenderCore/DX12/SR_RenderDevice_DX12.h"
#endif

#if ENABLE_RENDERDOC_API
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

		if (mCache[f].use_count() <= 2) // The cache holds one ref
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
	SR_SwapChain* sc = GetSwapChain();
	WaitForFence(sc->GetLastFrameFence());

	sc->Present();

	mTempResourceHeap->EndFrame();
}

SC_Ref<SR_CommandList> SR_RenderDevice::CreateCommandList(const SR_CommandListType& /*aType*/)
{
	assert(false && "Not implemented yet!");
	return nullptr;
}

SC_Ref<SR_Texture> SR_RenderDevice::CreateTexture(const SR_TextureProperties& /*aTextureProperties*/, const SC_Ref<SR_TextureResource>& /*aResource*/)
{
	assert(false && "Not implemented yet!");
	return nullptr;
}

SC_Ref<SR_RenderTarget> SR_RenderDevice::CreateRenderTarget(const SR_RenderTargetProperties& /*aRenderTargetProperties*/, const SC_Ref<SR_TextureResource>& /*aResource*/)
{
	assert(false && "Not implemented yet!");
	return nullptr;
}

SC_Ref<SR_DepthStencil> SR_RenderDevice::CreateDepthStencil(const SR_DepthStencilProperties& /*aDepthStencilProperties*/, const SC_Ref<SR_TextureResource>& /*aResource*/)
{
	assert(false && "Not implemented yet!");
	return nullptr;
}

SC_Ref<SR_TextureResource> SR_RenderDevice::CreateTextureResource(const SR_TextureResourceProperties& /*aTextureResourceProperties*/, const SR_PixelData* /*aInitialData = nullptr*/, uint32 /*aDataCount = 0*/)
{
	assert(false && "Not implemented yet!");
	return nullptr;
}

SR_TempTexture SR_RenderDevice::CreateTempTexture(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture, bool aIsRenderTarget, bool aIsWritable)
{
	assert(mTempResourceHeap);
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
	assert(false && "Not implemented yet!");
	return nullptr;
}

SC_Ref<SR_BufferResource> SR_RenderDevice::CreateBufferResource(const SR_BufferResourceProperties& /*aBufferResourceProperties*/, const void* /*aInitialData*/)
{
	assert(false && "Not implemented yet!");
	return nullptr;
}

SR_TempBuffer SR_RenderDevice::CreateTempBuffer(const SR_BufferResourceProperties& aBufferResourceProperties, bool aIsWritable)
{
	assert(mTempResourceHeap);
	return mTempResourceHeap->GetBuffer(aBufferResourceProperties, aIsWritable);
}

SC_Ref<SR_Heap> SR_RenderDevice::CreateHeap(const SR_HeapProperties& /*aHeapProperties*/)
{
	assert(false && "Not implemented yet!");
	return nullptr;
}

bool SR_RenderDevice::CompileShader(const SR_ShaderCompileArgs& /*aArgs*/, SR_ShaderByteCode& /*aOutByteCode*/, SR_ShaderMetaData* /*aOutMetaData*/)
{
	assert(false && "Not implemented yet!");
	return false;
}

bool SR_RenderDevice::CompileShader(const std::string& /*aShadercode*/, const SR_ShaderCompileArgs& /*aArgs*/, SR_ShaderByteCode& /*aOutByteCode*/, SR_ShaderMetaData* /*aOutMetaData*/)
{
	assert(false && "Not implemented yet!");
	return false;
}

SC_Ref<SR_ShaderState> SR_RenderDevice::CreateShaderState(const SR_ShaderStateProperties& /*aProperties*/)
{
	assert(false && "Not implemented yet!");
	return nullptr;
}

SC_Ref<SR_SwapChain> SR_RenderDevice::CreateSwapChain(const SR_SwapChainProperties& /*aProperties*/, void* /*aNativeWindowHandle*/)
{
	assert(false && "Not implemented yet!");
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
	assert(false && "Not implemented yet!");
	return nullptr;
}

SR_CommandQueue* SR_RenderDevice::GetCommandQueue(const SR_CommandListType& /*aType*/) const
{
	assert(false && "Not implemented yet!");
	return nullptr;
}

SR_DescriptorHeap* SR_RenderDevice::GetDefaultDescriptorHeap() const
{
	assert(false && "Not implemented yet!");
	return nullptr;
}

SR_DescriptorHeap* SR_RenderDevice::GetSamplerDescriptorHeap() const
{
	return nullptr;
}

SR_DescriptorHeap* SR_RenderDevice::GetRTVDescriptorHeap() const
{
	assert(false && "Not implemented yet!");
	return nullptr;
}

SR_DescriptorHeap* SR_RenderDevice::GetDSVDescriptorHeap() const
{
	assert(false && "Not implemented yet!");
	return nullptr;
}

SR_RootSignature* SR_RenderDevice::GetRootSignature(const SR_RootSignatureType& aType) const
{
	return mRootSignatures[static_cast<uint32>(aType)].get();
}

void SR_RenderDevice::SetSwapChain(SR_SwapChain* aSwapChain)
{
	mDefaultSwapChain.reset(aSwapChain);
}

SR_SwapChain* SR_RenderDevice::GetSwapChain() const
{
	return mDefaultSwapChain.get();
}

SR_InstanceBuffer* SR_RenderDevice::GetPersistentResourceInfo() const
{
	return mPersistentResourceInfo.get();
}

SR_RenderTaskManager* SR_RenderDevice::GetRenderTaskManager() const
{
	return mRenderTaskManager.get();
}

SC_Ref<SR_CommandList> SR_RenderDevice::GetTaskCommandList()
{
	if (!mRenderTaskManager)
		return nullptr;

	return mRenderTaskManager->GetCommandList(mRenderTaskManager->gCurrentTaskType);
}

const SR_RenderSupportCaps& SR_RenderDevice::GetSupportCaps() const
{
	return mSupportCaps;
}

#if ENABLE_RENDERDOC_API
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

const SR_API& SR_RenderDevice::GetAPI() const
{
	return mRenderAPIType;
}

bool SR_RenderDevice::Create(const SR_API& aAPIType)
{
	if (gInstance)
	{
		assert(false && "RenderDevice can only be created once!");
		return false;
	}

	switch (aAPIType)
	{
#if ENABLE_DX12
	case SR_API::D3D12:
		gInstance = new SR_RenderDevice_DX12();
		break;
#endif
#if ENABLE_VULKAN
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
#if ENABLE_RENDERDOC_API
	, mEnableRenderDocCaptures(false)
#endif
	, mRenderAPIType(aAPI)
{
	assert(gInstance == nullptr && "Only 1 render device may exist!");
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

#if ENABLE_RENDERDOC_API
	if (mEnableRenderDocCaptures)
	{
		if (HMODULE renderDocModule = GetModuleHandleA("renderdoc.dll"))
		{
			pRENDERDOC_GetAPI GetRenderDocAPI = (pRENDERDOC_GetAPI)GetProcAddress(renderDocModule, "RENDERDOC_GetAPI");
			int result = GetRenderDocAPI(eRENDERDOC_API_Version_1_4_2, (void**)&mRenderDocAPI);
			assert(result == 1);
		}
	}
	else
		mRenderDocAPI = nullptr;
#endif

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

	mRenderTaskManager = SC_MakeUnique<SR_RenderTaskManager>();
	if (!mRenderTaskManager->Init())
		return false;

	return true;
}
