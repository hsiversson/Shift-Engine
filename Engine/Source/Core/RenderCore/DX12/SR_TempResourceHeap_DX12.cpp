#include "SR_TempResourceHeap_DX12.h"

#if ENABLE_DX12
#include "SR_RenderDevice_DX12.h"
#include "SR_Heap_DX12.h"
#include "SR_TextureResource_DX12.h"
#include "SR_BufferResource_DX12.h"

SR_TempResourceHeap_DX12::SR_TempResourceHeap_DX12()
{

}

SR_TempResourceHeap_DX12::~SR_TempResourceHeap_DX12()
{

}

bool SR_TempResourceHeap_DX12::Init()
{
	SR_HeapProperties heapProps;
	heapProps.mByteSize = GB(1);
	heapProps.mResourceType = SR_HeapResourceType::RenderTarget;
	heapProps.mType = SR_HeapType::Default;
	heapProps.mDebugName = "SR_TempResourceHeap_DX12::RenderTarget_Heap";
	mResourceHeap_RT_DS_Textures = SC_MakeUnique<SR_Heap_DX12>(heapProps);
	if (!mResourceHeap_RT_DS_Textures->Init())
	{
		SC_ASSERT(false, "Could not create heap.");
		return false;
	}

	heapProps.mByteSize = MB(256);
	heapProps.mResourceType = SR_HeapResourceType::Texture;
	heapProps.mDebugName = "SR_TempResourceHeap_DX12::RWTexture_Heap";
	mResourceHeap_RW_Textures = SC_MakeUnique<SR_Heap_DX12>(heapProps);
	if (!mResourceHeap_RW_Textures->Init())
	{
		SC_ASSERT(false, "Could not create heap.");
		return false;
	}
	heapProps.mDebugName = "SR_TempResourceHeap_DX12::Texture_Heap";
	mResourceHeap_R_Textures = SC_MakeUnique<SR_Heap_DX12>(heapProps);
	if (!mResourceHeap_R_Textures->Init())
	{
		SC_ASSERT(false, "Could not create heap.");
		return false;
	}

	heapProps.mByteSize = MB(128);
	heapProps.mResourceType = SR_HeapResourceType::Buffer;
	heapProps.mType = SR_HeapType::Upload;
	heapProps.mDebugName = "SR_TempResourceHeap_DX12::ConstantBuffer_Heap";
	mResourceHeap_ConstantBuffers = SC_MakeUnique<SR_Heap_DX12>(heapProps);
	if (!mResourceHeap_ConstantBuffers->Init())
	{
		SC_ASSERT(false, "Could not create heap.");
		return false;
	}

	heapProps.mType = SR_HeapType::Default;
	heapProps.mDebugName = "SR_TempResourceHeap_DX12::Buffer_Heap";
	mResourceHeap_GenericBuffers = SC_MakeUnique<SR_Heap_DX12>(heapProps);
	if (!mResourceHeap_GenericBuffers->Init())
	{
		SC_ASSERT(false, "Could not create heap.");
		return false;
	}
	return true;
}

void SR_TempResourceHeap_DX12::EndFrameInternal()
{
	mResourceHeap_RW_Textures->EndFrame();
	mResourceHeap_ConstantBuffers->EndFrame();
	mResourceHeap_GenericBuffers->EndFrame();
}

SR_TempTexture SR_TempResourceHeap_DX12::GetTextureInternal(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture, bool aIsRenderTarget, bool aIsWritable)
{
	SR_TextureResourceProperties resourceProps(aTextureProperties);
	resourceProps.mHeap = mResourceHeap_RW_Textures.get();
	resourceProps.mAllowRenderTarget = resourceProps.mAllowRenderTarget && aIsRenderTarget;
	resourceProps.mAllowUnorderedAccess = resourceProps.mAllowUnorderedAccess && aIsWritable;

	SR_TempTexture tempTexture = {};
	SC_Ref<SR_TextureResource_DX12> dx12Resource = SC_MakeRef<SR_TextureResource_DX12>(resourceProps);
	if (dx12Resource->Init(nullptr, 0))
	{
		tempTexture.mResource = dx12Resource;

		if (aIsTexture)
		{
			SR_TextureProperties texProps(resourceProps.mFormat);
			tempTexture.mTexture = SR_RenderDevice_DX12::gInstance->CreateTexture(texProps, tempTexture.mResource);
		}

		if (aIsRenderTarget)
		{
			SR_RenderTargetProperties rtProps(resourceProps.mFormat);
			tempTexture.mRenderTarget = SR_RenderDevice_DX12::gInstance->CreateRenderTarget(rtProps, tempTexture.mResource);
		}

		if (aIsWritable)
		{
			SR_TextureProperties texProps(resourceProps.mFormat);
			texProps.mWritable = true;
			tempTexture.mRWTexture = SR_RenderDevice_DX12::gInstance->CreateTexture(texProps, tempTexture.mResource);
		}
	}
	
	return tempTexture;
}

SR_TempBuffer SR_TempResourceHeap_DX12::GetBufferInternal(const SR_BufferResourceProperties& aBufferProperties, bool aIsWritable)
{
	const bool isConstantBuffer = (aBufferProperties.mBindFlags & SR_BufferBindFlag_ConstantBuffer);
	SR_BufferResourceProperties resourceProps(aBufferProperties);

	if (isConstantBuffer)
		resourceProps.mHeap = mResourceHeap_ConstantBuffers.get();
	else
		resourceProps.mHeap = mResourceHeap_GenericBuffers.get();

	resourceProps.mWritable = resourceProps.mWritable && aIsWritable;

	SR_TempBuffer tempBuffer = {};
	SC_Ref<SR_BufferResource_DX12> dx12Resource = SC_MakeRef<SR_BufferResource_DX12>(resourceProps);
	if (dx12Resource->Init(aBufferProperties.mInitialData))
	{
		tempBuffer.mResource = dx12Resource;

		if (!isConstantBuffer)
		{
			SR_BufferProperties bufferProps;
			bufferProps.mElementCount = resourceProps.mElementCount;
			bufferProps.mType = SR_BufferType::Bytes;
			tempBuffer.mBuffer = SR_RenderDevice_DX12::gInstance->CreateBuffer(bufferProps, tempBuffer.mResource);
		}

		//if (aIsWritable)
		//{
		//	SR_BufferProperties bufferProps;
		//	bufferProps.mElementCount = resourceProps.mElementCount;
		//	tempBuffer.mRWBuffer = SR_RenderDevice_DX12::gD3D12Instance->CreateBuffer(bufferProps, tempBuffer.mResource);
		//}
	}
	else
	{
		SC_ASSERT(false, "Failed to init buffer.");
	}

	return tempBuffer;
}

#endif //ENABLE_DX12