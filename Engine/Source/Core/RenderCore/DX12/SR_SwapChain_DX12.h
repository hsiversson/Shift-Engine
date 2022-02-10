#pragma once

#if ENABLE_DX12
#include "RenderCore/Interface/SR_SwapChain.h"
#include "SR_RenderTarget_DX12.h"
#include "SR_TextureResource_DX12.h"

struct IDXGIFactory1;
struct IDXGIFactory4;
struct IDXGISwapChain1;
struct IDXGISwapChain4;
struct ID3D12Resource;

class SR_RenderDevice_DX12;

class SR_SwapChain_DX12 : public SR_SwapChain
{
public:
	SR_SwapChain_DX12(SR_RenderDevice_DX12* aDevice);
	~SR_SwapChain_DX12();

	bool Init(const SR_SwapChainProperties& aProperties, void* aWindowHandle);

	void Update(const SR_SwapChainProperties& aProperties) override;

	void Present() override;

private:

	bool CreateSwapChain();
	bool CreateResources();
	void DestroyResources();

	void UpdateInternal();

#if IS_DESKTOP_PLATFORM
	bool InitFrameLatencyWaitable();
#endif

	SR_ComPtr<IDXGIFactory1> mDXGIFactory;
	SR_ComPtr<IDXGIFactory4> mDXGIFactory4;
	SR_ComPtr<IDXGIFactory6> mDXGIFactory6;
	SR_ComPtr<IDXGISwapChain1> mDXGISwapChain;
	SR_ComPtr<IDXGISwapChain4> mDXGISwapChain4;
#if IS_DESKTOP_PLATFORM
	HANDLE myFrameLatencyWaitableObject;
#endif

	SR_RenderDevice_DX12* mRenderDevice;

	void* mWindowHandle;
	uint32 mSwapChainFlags;
};

#endif
