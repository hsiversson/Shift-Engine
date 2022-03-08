
#include "SR_SwapChain_DX12.h"

#if ENABLE_DX12

#include "SR_RenderDevice_DX12.h"
#include "SR_CommandQueue_DX12.h"
#include "SR_Texture_DX12.h"
#include "SR_RenderTarget_DX12.h"
#include "SR_TextureResource_DX12.h"
#include <string>

SR_SwapChain_DX12::SR_SwapChain_DX12(SR_RenderDevice_DX12* aDevice)
	: mRenderDevice(aDevice)
#if IS_DESKTOP_PLATFORM
	, myFrameLatencyWaitableObject(nullptr)
#endif
	, mWindowHandle(nullptr)
	, mSwapChainFlags(0)
{

}

SR_SwapChain_DX12::~SR_SwapChain_DX12()
{

}

bool SR_SwapChain_DX12::Init(const SR_SwapChainProperties& aProperties, void* aWindowHandle)
{
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&mDXGIFactory));
	if (!VerifyHRESULT(hr))
		return false;

	hr = mDXGIFactory.As(&mDXGIFactory4);
	if (!VerifyHRESULT(hr))
		return false;

	hr = mDXGIFactory.As(&mDXGIFactory6);
	if (!VerifyHRESULT(hr))
		return false;
	
	mProperties = aProperties;
	mWindowHandle = aWindowHandle;
	
	if (!CreateSwapChain())
	{
		return false;
	}

	if (!CreateResources())
	{
		return false;
	}

	return true;
}

void SR_SwapChain_DX12::Update(const SR_SwapChainProperties& aProperties)
{
	if (!mDXGISwapChain)
		return;

	mProperties = aProperties;
	UpdateInternal();

}

void SR_SwapChain_DX12::Present()
{
	if (!mDXGISwapChain)
		return;

	if (mProperties.mFrameLatency && myFrameLatencyWaitableObject)
	{
		while (WaitForSingleObject(myFrameLatencyWaitableObject, 0) == S_OK);
	}

	HRESULT hr = S_FALSE;
	uint32 flags = 0;
	if (!mProperties.mFullscreen && (mSwapChainFlags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING))
		flags |= DXGI_PRESENT_ALLOW_TEARING;

	hr = mDXGISwapChain->Present(0, flags);
	mFrameFence[mCurrentIndex] = SR_RenderDevice::gInstance->GetGraphicsCommandQueue()->InsertFence();

	mCurrentIndex = (uint8)mDXGISwapChain4->GetCurrentBackBufferIndex();
	mCurrentResource = &mBackbufferResources[mCurrentIndex];
}

bool SR_SwapChain_DX12::CreateSwapChain()
{
	//ID3D12Device* device = mRenderDevice->GetD3D12Device();

	bool allowTearing = false;
	HRESULT hr = mDXGIFactory6->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));

	mNumBackbuffers = (mProperties.mTripleBuffer && (mProperties.mFullscreen)) ? 3 : 2;

	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width = mProperties.mSize.x;
	desc.Height = mProperties.mSize.y;
	desc.Format = SR_D3D12ConvertFormat(mProperties.mFormat);
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = (uint32)mNumBackbuffers;
	desc.Stereo = false;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if (!mProperties.mFullscreen)
		desc.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
	if (!allowTearing)
		desc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	mSwapChainFlags = desc.Flags; // Store flags for internal swapchain updates

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
	fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	fullscreenDesc.Windowed = true;
	fullscreenDesc.RefreshRate.Numerator = 0;
	fullscreenDesc.RefreshRate.Denominator = 1;

	SR_CommandQueue_DX12* cmdQueue = static_cast<SR_CommandQueue_DX12*>(mRenderDevice->GetGraphicsCommandQueue());
	hr = mDXGIFactory4->CreateSwapChainForHwnd(cmdQueue->GetD3D12CommandQueue(), HWND(mWindowHandle), &desc, &fullscreenDesc, nullptr, &mDXGISwapChain);
	if (!VerifyHRESULT(hr))
		return false;

	hr = mDXGIFactory6->MakeWindowAssociation(HWND(mWindowHandle), DXGI_MWA_NO_WINDOW_CHANGES);
	if (!VerifyHRESULT(hr))
		return false;

	hr = mDXGISwapChain.As(&mDXGISwapChain4);
	if (!VerifyHRESULT(hr))
		return false;


#if IS_DESKTOP_PLATFORM
	return InitFrameLatencyWaitable();
#else
	return true;
#endif
}

bool SR_SwapChain_DX12::CreateResources()
{
	DestroyResources();

	if (mDXGISwapChain)
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		mDXGISwapChain->GetDesc(&swapChainDesc);

		for (uint32 i = 0; i < swapChainDesc.BufferCount; ++i)
		{
			ID3D12Resource* res;
			HRESULT hr = mDXGISwapChain->GetBuffer(i, IID_PPV_ARGS(&res));
			if (!VerifyHRESULT(hr))
			{
				assert(false);
				return false;
			}

			std::wstring name(L"Framebuffer Texture " + std::to_wstring(i));
			res->SetName(name.c_str());

			D3D12_RESOURCE_DESC desc = res->GetDesc();

			SR_TextureResourceProperties framebufferProperties;
			framebufferProperties.mSize.x = (int)desc.Width;
			framebufferProperties.mSize.y = (int)desc.Height;
			framebufferProperties.mNumMips = desc.MipLevels;
			framebufferProperties.mFormat = SR_D3D12ConvertFormat(desc.Format);
			framebufferProperties.mAllowRenderTarget = true;
			framebufferProperties.mAllowUnorderedAccess = true;
			framebufferProperties.mType = SR_ResourceType::Texture2D;
			mBackbufferResources[i].mResource = SC_MakeRef<SR_TextureResource_DX12>(framebufferProperties, res);
			mBackbufferResources[i].mResource->mLatestResourceState = SR_ResourceState_Present;

			SR_RenderTargetProperties renderTargetProperties(framebufferProperties.mFormat);
			mBackbufferResources[i].mRenderTarget = SR_RenderDevice_DX12::gD3D12Instance->CreateRenderTarget(renderTargetProperties, mBackbufferResources[i].mResource);

			SR_TextureProperties texProperties(framebufferProperties.mFormat);
			mBackbufferResources[i].mTexture = SR_RenderDevice_DX12::gD3D12Instance->CreateTexture(texProperties, mBackbufferResources[i].mResource);
		}

		mCurrentResource = &mBackbufferResources[0];
	}

	return true;
}

void SR_SwapChain_DX12::DestroyResources()
{
	if (!SR_RenderDevice_DX12::gD3D12Instance)
		return;


	for (uint32 i = 0; i < 3; ++i)
	{
		SR_RenderDevice_DX12::gD3D12Instance->WaitForFence(mFrameFence[i]);
		mBackbufferResources[i].mRenderTarget = nullptr;
		mBackbufferResources[i].mTexture = nullptr;
		mBackbufferResources[i].mResource = nullptr;
	}

	mCurrentResource = nullptr;
}

void SR_SwapChain_DX12::UpdateInternal()
{
	DestroyResources();
	myFrameLatencyWaitableObject = nullptr;

	if (mDXGISwapChain)
	{
		HRESULT hr = mDXGISwapChain->ResizeBuffers(mProperties.mTripleBuffer ? 3 : 2, mProperties.mSize.x, mProperties.mSize.y, SR_D3D12ConvertFormat(mProperties.mFormat), mSwapChainFlags); 
		if (!VerifyHRESULT(hr))
		{
			SC_ASSERT(false);
			return;
		}
	}

	CreateResources();
}

#if IS_DESKTOP_PLATFORM
bool SR_SwapChain_DX12::InitFrameLatencyWaitable()
{
	myFrameLatencyWaitableObject = nullptr;

	if (!mDXGISwapChain4)
		return false;

	if (mDXGISwapChain4 && mProperties.mFrameLatency && (mSwapChainFlags & DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT))
	{
		myFrameLatencyWaitableObject = mDXGISwapChain4->GetFrameLatencyWaitableObject();
		mDXGISwapChain4->SetMaximumFrameLatency(mProperties.mFrameLatency);

		if (myFrameLatencyWaitableObject)
		{
			while (WaitForSingleObject(myFrameLatencyWaitableObject, 0) == S_OK);
		}
	}
	else
		mDXGISwapChain4->SetMaximumFrameLatency(2);

	return true;
}
#endif

#endif