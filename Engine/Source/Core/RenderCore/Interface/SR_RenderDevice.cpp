#include "SR_RenderDevice.h"

#if SR_ENABLE_DX12
#include "RenderCore/DX12/SR_RenderDevice_DX12.h"
#endif

#if SR_ENABLE_RENDERDOC_API
#include "renderdoc_app.h"
#endif

#include "RenderCore/Resources/SR_RingBuffer.h"
#include "RenderCore/ImGui/SR_ImGui.h"

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

		if (mCache[f]->GetReferenceCount() <= 2) // The cache holds one ref
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
uint32 SR_RenderDevice::gFrameCounter;
uint32 SR_RenderDevice::gLatestFinishedFrame;

SR_RenderDevice::~SR_RenderDevice()
{
	for (TempRingBuffers& ringBuffers : mTempRingBuffers)
		ringBuffers.mRingBuffers.RemoveAll();
}

struct FrameFence
{
	SC_Ref<SR_TaskEvent> mFence;
	uint32 mFrameIndex;
};
static SC_RingArray<FrameFence> locFrameSyncEvents;
uint32 locLastEndFrameIndex;

SC_Ref<SR_TaskEvent> SR_RenderDevice::BeginFrame(uint32 aFrameIndex)
{
	//SC_Event event;
	auto beginFrame = [this, aFrameIndex]()
	{
		gFrameCounter = aFrameIndex;
		std::string tag = SC_FormatStr("Render Frame {}", gFrameCounter);
		SC_PROFILER_BEGIN_SESSION(tag);
		SC_PROFILER_FUNCTION();

		static constexpr uint32 maxFrameDelay = 1;

		while (locFrameSyncEvents.Count() <= maxFrameDelay)
		{
			SC_Ref<SR_TaskEvent> fence = SC_MakeRef<SR_TaskEvent>();
			fence->mFence = InsertFence(SR_CommandListType::Graphics);
			fence->mCPUEvent.Signal();

			FrameFence frameFence = { fence, locLastEndFrameIndex };
			locFrameSyncEvents.Add(frameFence);
		}

		if (locFrameSyncEvents.Count())
		{
			FrameFence& fence = locFrameSyncEvents.Peek();
			if (fence.mFence)
			{
				SC_Timer timer;
				WaitForFence(fence.mFence->mFence);
				gLatestFinishedFrame = SC_Max(gLatestFinishedFrame, fence.mFrameIndex);
			}
			locFrameSyncEvents.Remove();
		}

		for (uint32 i = 0; i < locFrameSyncEvents.Count(); ++i)
		{
			FrameFence& fence = locFrameSyncEvents[i];
			if (fence.mFence && !fence.mFence->mFence.IsPending())
			{
				fence.mFence.Reset();
				gLatestFinishedFrame = SC_Max(gLatestFinishedFrame, fence.mFrameIndex);
			}
		}
	};
	mBeginFrameEvent = PostGraphicsTask(beginFrame);
	return mBeginFrameEvent;
}

void SR_RenderDevice::Present()
{
	auto presentTask = [this]()
	{
		GetQueueManager()->Flush();
		std::string tag = SC_FormatStr("SR_RenderDevice::Present (frame: {})", gFrameCounter);
		SC_PROFILER_EVENT_START(tag.c_str());
		SR_SwapChain* sc = GetSwapChain();
		sc->Present();
		//SC_Thread::Sleep(16);
		SC_PROFILER_EVENT_END();
	};

	mPresentEvent = PostGraphicsTask(presentTask);
	mPresentEvent->mCPUEvent.Wait();
}

void SR_RenderDevice::EndFrame()
{
	auto endFrame = [this]()
	{
		{
			SC_PROFILER_FUNCTION();
			locLastEndFrameIndex = gFrameCounter;
			mTempResourceHeap->EndFrame();
			GarbageCollect();
			mDelayDestructor->Run();
		}
		std::string tag = SC_FormatStr("Render Frame {}", gFrameCounter);
		SC_PROFILER_END_SESSION(tag);
	};
	mEndFrameEvent = PostGraphicsTask(endFrame);
}

void SR_RenderDevice::GarbageCollect()
{
	uint32 currentFrameIndex = SC_Time::gFrameCounter;
	const uint32 maxFailedLocks = 3;
	//const uint32 maxAgeInSeconds = 15;
	uint32 maxAge = 10;

	uint32 releaseFrame = currentFrameIndex > maxAge ? currentFrameIndex - maxAge : 0;

	for (uint32 i = 0, e = static_cast<uint32>(TempRingBufferType::COUNT); i < e; ++i)
	{
		TempRingBuffers& ringBuffers = mTempRingBuffers[i];

		if (ringBuffers.mRingBuffers.IsEmpty())
			continue;

		bool isOverBudget = false;

		SC_MutexLock lock;
		if (!lock.TryLock(ringBuffers.mMutex))
		{
			++ringBuffers.mNumFailedMutexLocks;
			if (ringBuffers.mNumFailedMutexLocks > maxFailedLocks)
				lock.Lock(ringBuffers.mMutex);
			else 
				continue;
		}
		ringBuffers.mNumFailedMutexLocks = 0;

		bool wasPreviousAlive = true;
		for (uint32 idx = 0; idx < ringBuffers.mRingBuffers.Count(); )
		{
			SR_RingBuffer& ringBuffer = ringBuffers.mRingBuffers[idx];
			bool isAlive = ringBuffer.mLatestAllocFrame >= releaseFrame || (ringBuffer.mLatestAllocFence.mType != SR_CommandListType::Unknown && IsFencePending(ringBuffer.mLatestAllocFence));
			bool wasRemoved = false;

			if (!isAlive)
			{
				if (!wasPreviousAlive || isOverBudget)
				{
					ringBuffers.mRingBuffers.RemoveAt(idx);
					wasRemoved = true;

					if (!isOverBudget)
						return;
				}
			}

			wasPreviousAlive = isAlive;

			if (!wasRemoved)
				++idx;
		}
	}
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

SR_BufferResource* SR_RenderDevice::GetTempBufferResource(uint64& aOutOffset, SR_BufferBindFlag aBufferType, uint32 aByteSize, const void* aInitialData, uint32 aAlignment, const SR_Fence& aCompletionFence)
{
	SC_MutexLock lock;
	//bool isTaskThread = SC_Thread::gIsTaskThread;

	TempRingBuffers* tempRingBuffers = nullptr;
	uint32 size = 0;
	uint32 bindFlags = 0;
	uint32 defaultRingBufferSize = MB(1);
	uint32 alignment = 256;
	size_t maxNormalRingSize = 0;
	const char* name = nullptr;

	if (aBufferType == SR_BufferBindFlag_ConstantBuffer)
	{
		tempRingBuffers = &mTempRingBuffers[static_cast<uint32>(TempRingBufferType::Constant)];
		size = SC_Align(aByteSize, 256);
		bindFlags = SR_BufferBindFlag_ConstantBuffer;
		name = "Constant Ring Buffer";
	}
	else if (aBufferType == SR_BufferBindFlag_Staging)
	{
		tempRingBuffers = &mTempRingBuffers[static_cast<uint32>(TempRingBufferType::Staging)];
		size = SC_Align(aByteSize, 256);
		bindFlags = SR_BufferBindFlag_Staging;
		name = "Staging Ring Buffer";
	}
	else if (aBufferType == SR_BufferBindFlag_Buffer)
	{
		tempRingBuffers = &mTempRingBuffers[static_cast<uint32>(TempRingBufferType::Buffer)];
		size = SC_Align(aByteSize, 256);
		bindFlags = SR_BufferBindFlag_Buffer;
		name = "Buffers Ring Buffer";
	}
	else if (aBufferType == SR_BufferBindFlag_VertexBuffer || aBufferType == SR_BufferBindFlag_IndexBuffer)
	{
		tempRingBuffers = &mTempRingBuffers[static_cast<uint32>(TempRingBufferType::VertexIndex)];
		size = SC_Align(aByteSize, 64);
		bindFlags = SR_BufferBindFlag_VertexBuffer | SR_BufferBindFlag_IndexBuffer;
		name = "Vertex/Index Ring Buffer";
	}
	else
	{
		SC_ASSERT(false, "No temp allocation supported for this buffer type.");
		return nullptr;
	}

	uint32 maxMisalignment = 0;
	if (aAlignment > 1)
	{
		if (!SC_IsPow2(aAlignment))
			maxMisalignment = aAlignment - 1;
		else if (aAlignment > alignment)
			maxMisalignment = aAlignment - alignment;

		size = SC_Align(aByteSize + maxMisalignment, alignment);
	}

	SR_BufferResource* bufferResource = nullptr;
	uint64 currentRingSize = 0;

	lock.Lock(tempRingBuffers->mMutex);

	bool isCurrentRingSizeSufficient = false;

	for (SR_RingBuffer& ringBuffer : tempRingBuffers->mRingBuffers)
	{
		if (ringBuffer.GetOffset(aOutOffset, size, 0, aCompletionFence))
		{
			bufferResource = ringBuffer.GetBufferResource();
			break;
		}

		if (size <= ringBuffer.mAllocBlockMaxSize)
			isCurrentRingSizeSufficient = true;

		currentRingSize += ringBuffer.mSize;
	}

	if (!bufferResource)
	{
		SC_ASSERT(tempRingBuffers->mRingBuffers.Count() < 200);

		uint32 ringBufferSize = defaultRingBufferSize;
		SC_ASSERT(size <= 0x10000000); // logic below will overflow 32-bit uint if size is larger than this (256 MB)
		uint32 sizeWithSafety = SC_GetNextPow2(size * 4);

		if (maxNormalRingSize && sizeWithSafety > maxNormalRingSize)
			sizeWithSafety = SC_GetNextPow2(size);

		if (sizeWithSafety > ringBufferSize)
			ringBufferSize = sizeWithSafety;

		SR_BufferResourceProperties props;
		props.mBindFlags = bindFlags;
		props.mElementCount = ringBufferSize;
		props.mElementSize = 1;
		props.mDebugName = name;
		props.mIsUploadBuffer = true;

		SC_Ref<SR_BufferResource> newBufferResource = CreateBufferResource(props, nullptr);
		bufferResource = newBufferResource;

		SR_RingBuffer& ring = tempRingBuffers->mRingBuffers.Add(SR_RingBuffer(newBufferResource, alignment));
		if (ring.mAllocBlockMaxSize < size)
			ring.mAllocBlockMaxSize = ringBufferSize;

		bool ok = ring.GetOffset(aOutOffset, size, 0, aCompletionFence);
		SC_ASSERT(ok, "Failed to allocate from new ring buffer");
	}

	if (maxMisalignment)
	{
		uint32 misalignment = aOutOffset & aAlignment;
		SC_ASSERT(misalignment <= maxMisalignment);

		if (misalignment)
			aOutOffset += aAlignment - misalignment;

		SC_ASSERT((aOutOffset % aAlignment) == 0);
	}

	if (aByteSize && aInitialData)
	{
		lock.Unlock();
		bufferResource->UpdateData((uint32)aOutOffset, aInitialData, aByteSize);
	}

	return bufferResource;
}

SC_Ref<SR_Heap> SR_RenderDevice::CreateHeap(const SR_HeapProperties& /*aHeapProperties*/)
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

bool SR_RenderDevice::IsFencePending(const SR_Fence& aFence) const
{
	if (aFence.mType == SR_CommandListType::Unknown)
		return false;

	return mQueueManager->IsFencePending(aFence);
}

bool SR_RenderDevice::WaitForFence(const SR_Fence& aFence, bool aBlock)
{
	if (aFence.mType == SR_CommandListType::Unknown)
		return false;

	return mQueueManager->WaitForFence(aFence, aBlock);
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

SC_Ref<SR_TaskEvent> SR_RenderDevice::PostGraphicsTask(SR_RenderTaskSignature aTask)
{
	SC_Ref<SR_TaskEvent> taskEvent = mContextThreads[static_cast<uint32>(SR_CommandListType::Graphics)]->PostTask(aTask);
	return taskEvent;
}

SC_Ref<SR_TaskEvent> SR_RenderDevice::PostComputeTask(SR_RenderTaskSignature aTask)
{
	SC_Ref<SR_TaskEvent> taskEvent = mContextThreads[static_cast<uint32>(SR_CommandListType::Compute)]->PostTask(aTask);
	return taskEvent;
}

SC_Ref<SR_TaskEvent> SR_RenderDevice::PostCopyTask(SR_RenderTaskSignature aTask)
{
	SC_Ref<SR_TaskEvent> taskEvent = mContextThreads[static_cast<uint32>(SR_CommandListType::Copy)]->PostTask(aTask);
	return taskEvent;
}

SR_Fence SR_RenderDevice::InsertFence(const SR_CommandListType& aContextType)
{
	SR_Fence fence = GetCommandQueue(aContextType)->GetNextFence();
	GetQueueManager()->SignalFence(aContextType, fence);
	return fence;
}

//SR_InstanceBuffer* SR_RenderDevice::GetPersistentResourceInfo() const
//{
//	return mPersistentResourceInfo.get();
//}

SR_QueueManager* SR_RenderDevice::GetQueueManager() const
{
	return mQueueManager.get();
}

SC_Ref<SR_CommandList> SR_RenderDevice::GetTaskCommandList()
{
	if (!mQueueManager)
		return nullptr;

	if (!SR_RenderThread::gCurrentCommandList)
		SR_RenderThread::gCurrentCommandList = mQueueManager->GetCommandList(SR_RenderThread::gCurrentContextType);

	return SR_RenderThread::gCurrentCommandList;
}

SR_ResourceDelayDestructor* SR_RenderDevice::GetDelayDestructor() const
{
	return mDelayDestructor.get();
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
	: mDefaultSwapChain(nullptr)
	, mEnableDebugMode(false)
	, mBreakOnError(false)
	, mEnableGpuValidation(false)
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
	if (SC_CommandLine::HasCommand("gpuvalidation"))
	{
		mEnableDebugMode = true;
		mEnableGpuValidation = true;
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
	//mPersistentResourceInfo = SC_MakeUnique<SR_InstanceBuffer>();
	//if (!mPersistentResourceInfo->Init(32768))
	//{
	//	return false;
	//}

	mDelayDestructor = SC_MakeUnique<SR_ResourceDelayDestructor>();

	mQueueManager = SC_MakeUnique<SR_QueueManager>();
	if (!mQueueManager->Init())
		return false;

	for (uint32 i = 0; i < static_cast<uint32>(SR_CommandListType::COUNT); ++i)
		mContextThreads[i] = SC_MakeUnique<SR_RenderThread>(static_cast<SR_CommandListType>(i));

	return true;
}
