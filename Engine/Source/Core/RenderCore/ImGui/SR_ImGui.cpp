#include "SR_ImGui.h"
#include "Interface/SR_RenderDevice.h"

#if IS_WINDOWS_PLATFORM
#include "DX12/SR_RenderDevice_DX12.h"
#include "DX12/SR_RootSignature_DX12.h"
#endif

#include "imgui.h"

static const char* GetVertexShaderCode()
{
	return
		"cbuffer Constants : register(b0) \n"
		"{\n"
		"	float4x4 ProjectionMatrix;\n"
		"	uint FontTextureIndex;\n"
		"	float DPIScale;\n"
		"};\n"
		"struct VS_INPUT\n"
		"{\n"
		"	float2 pos : POSITION;\n"
		"	float4 col : COLOR0;\n"
		"	float2 uv : UV0;\n"
		"};\n"
		"struct PS_INPUT\n"
		"{\n"
		"	float4 pos : SV_POSITION;\n"
		"	float4 col : COLOR0; \n"
		"	float2 uv  : UV0;\n"
		"};\n"
		"PS_INPUT main(VS_INPUT input)\n"
		"{\n"
		"	PS_INPUT output;\n"
		"	output.pos = mul(ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f)) * DPIScale;\n"
		"	output.col = input.col;\n"
		"	output.uv  = input.uv;\n"
		"	return output;\n"
		"}\n";
}


static const char* GetPixelShaderCode()
{
	return
		"cbuffer Constants : register(b0) \n"
		"{\n"
		"	float4x4 ProjectionMatrix;\n"
		"	uint TextureIndex;\n"
		"};\n"
		"struct PS_INPUT\n"
		"{\n"
		"	float4 pos : SV_POSITION;\n"
		"	float4 col : COLOR0; \n"
		"	float2 uv  : UV0;\n"
		"}; \n"
		"SamplerState sampler0 : register(s0, space1);\n" // static samplers are in space1
		"float4 main(PS_INPUT input) : SV_Target0\n"
		"{\n"
		"	Texture2D texture0 = ResourceDescriptorHeap[TextureIndex];\n"
		"	float4 outCol = input.col * texture0.Sample(sampler0, input.uv);\n"
		"	return outCol; \n"
		"}\n";
}

struct SR_ImGuiRenderBuffers
{
	SC_Ref<SR_BufferResource> mVertexBuffer;
	SC_Ref<SR_BufferResource> mIndexBuffer;
};

struct SR_ImGuiViewportData
{
	static constexpr uint32 gNumFramesInFlight = 2;

	SR_ImGuiViewportData()
		: mCommandQueue(nullptr)
		, mCommandList(nullptr)
		, mDescriptorHeap(nullptr)
		, mSwapChain(nullptr)
		, mFrameIndex(0)
	{}

	SR_CommandQueue* mCommandQueue;
	SR_CommandList* mCommandList;
	SR_DescriptorHeap* mDescriptorHeap;
	SR_SwapChain* mSwapChain;

	SR_ImGuiRenderBuffers mRenderbuffers[gNumFramesInFlight];

	uint32 mFrameIndex;
};

struct SR_ImGuiVertexConstants
{
	SC_Matrix mMVP;
	uint32 mTextureIndex;
	float mDPIScale;
};

static void SR_ImGui_CreateWindow(ImGuiViewport* /*aViewport*/)
{

}

static void SR_ImGui_DestroyWindow(ImGuiViewport* /*aViewport*/)
{

}

static void SR_ImGui_SetWindowSize(ImGuiViewport* /*aViewport*/, ImVec2 /*aSize*/)
{

}


static void SR_ImGui_RenderWindow(ImGuiViewport* /*aViewport*/, void*)
{

}

static void SR_ImGui_SwapBuffers(ImGuiViewport* /*aViewport*/, void*)
{

}

static bool SR_ImGui_UpdateMouseCursor()
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
		return false;

	ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
	if (imguiCursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		::SetCursor(NULL);
	else
	{
		LPTSTR win32Cursor = IDC_ARROW;
		switch (imguiCursor)
		{
		case ImGuiMouseCursor_TextInput:    win32Cursor = IDC_IBEAM; break;
		case ImGuiMouseCursor_ResizeAll:    win32Cursor = IDC_SIZEALL; break;
		case ImGuiMouseCursor_ResizeEW:     win32Cursor = IDC_SIZEWE; break;
		case ImGuiMouseCursor_ResizeNS:     win32Cursor = IDC_SIZENS; break;
		case ImGuiMouseCursor_ResizeNESW:   win32Cursor = IDC_SIZENESW; break;
		case ImGuiMouseCursor_ResizeNWSE:   win32Cursor = IDC_SIZENWSE; break;
		case ImGuiMouseCursor_Hand:         win32Cursor = IDC_HAND; break;
		case ImGuiMouseCursor_NotAllowed:   win32Cursor = IDC_NO; break;
		default:
		case ImGuiMouseCursor_Arrow:        win32Cursor = IDC_ARROW; break;
		}
		::SetCursor(::LoadCursor(NULL, win32Cursor));
	}
	return true;
}

SR_ImGui* SR_ImGui::gInstance = nullptr;

SR_ImGui::SR_ImGui()
{
	assert(gInstance == nullptr);
	gInstance = this;
}

SR_ImGui::~SR_ImGui()
{
	gInstance = nullptr;
	Shutdown();
}

bool SR_ImGui::Init(void* aNativeWindowHandle, float aDPIScale)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	mDPIScale = aDPIScale;

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	SetStyle();
	io.FontDefault = io.Fonts->AddFontFromFileTTF((SC_EnginePaths::Get().GetEngineDataDirectory() + "/Fonts/OpenSans/OpenSans-Regular.ttf").GetStr(), 18.0f);
	io.FontGlobalScale = mDPIScale;

	io.BackendRendererName = "SR_ImGui_Renderer";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
		platformIO.Renderer_CreateWindow = SR_ImGui_CreateWindow;
		platformIO.Renderer_DestroyWindow = SR_ImGui_DestroyWindow;
		platformIO.Renderer_SetWindowSize = SR_ImGui_SetWindowSize;
		platformIO.Renderer_RenderWindow = SR_ImGui_RenderWindow;
		platformIO.Renderer_SwapBuffers = SR_ImGui_SwapBuffers;
	}

	ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	mainViewport->RendererUserData = IM_NEW(SR_ImGuiViewportData)();

	if (!InitPlatform(aNativeWindowHandle))
		return false;

	return InitRenderObjects();
}

void SR_ImGui::BeginFrame()
{
	SR_RenderDevice::gInstance->WaitForFence(mLastFence);

#if IS_WINDOWS_PLATFORM
	NewFrameWin64();
#else
#error Platform not supported!
#endif
	ImGui::NewFrame();

	mCommandList->Begin();
}

void SR_ImGui::Render(SR_RenderTarget* aRenderTarget)
{
	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();
	if (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f)
		return;


	ImGuiIO& io = ImGui::GetIO();
	drawData->ScaleClipRects(io.DisplayFramebufferScale);

	SR_ImGuiViewportData* renderData = (SR_ImGuiViewportData*)drawData->OwnerViewport->RendererUserData;
	++renderData->mFrameIndex;
	SR_ImGuiRenderBuffers& renderBuffer = renderData->mRenderbuffers[renderData->mFrameIndex % SR_ImGuiViewportData::gNumFramesInFlight];

	if (renderBuffer.mVertexBuffer == nullptr || renderBuffer.mVertexBuffer->GetProperties().mElementCount < (uint32)drawData->TotalVtxCount)
	{
		uint32 newSize = drawData->TotalVtxCount + 5000;

		SR_BufferResourceProperties newVertexBufferProps = (renderBuffer.mVertexBuffer) ? renderBuffer.mVertexBuffer->GetProperties() : SR_BufferResourceProperties();
		newVertexBufferProps.mElementCount = newSize;
		newVertexBufferProps.mElementSize = sizeof(ImDrawVert);
		newVertexBufferProps.mBindFlags = SR_BufferBindFlag_VertexBuffer;
		newVertexBufferProps.mIsUploadBuffer = true;
		renderBuffer.mVertexBuffer = SR_RenderDevice::gInstance->CreateBufferResource(newVertexBufferProps, nullptr);
	}
	if (renderBuffer.mIndexBuffer == nullptr || renderBuffer.mIndexBuffer->GetProperties().mElementCount < (uint32)drawData->TotalIdxCount)
	{
		uint32 newSize = drawData->TotalIdxCount + 10000;

		SR_BufferResourceProperties newIndexBufferProps = (renderBuffer.mIndexBuffer) ? renderBuffer.mIndexBuffer->GetProperties() : SR_BufferResourceProperties();
		newIndexBufferProps.mElementCount = newSize;
		newIndexBufferProps.mElementSize = sizeof(ImDrawIdx);
		newIndexBufferProps.mBindFlags = SR_BufferBindFlag_IndexBuffer;
		newIndexBufferProps.mIsUploadBuffer = true;
		renderBuffer.mIndexBuffer = SR_RenderDevice::gInstance->CreateBufferResource(newIndexBufferProps, nullptr);
	}

	ImDrawVert* vtxDst = (ImDrawVert*)renderBuffer.mVertexBuffer->GetDataPtr();
	ImDrawIdx* idxDst = (ImDrawIdx*)renderBuffer.mIndexBuffer->GetDataPtr();
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmdList = drawData->CmdLists[n];
		SC_Memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
		SC_Memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtxDst += cmdList->VtxBuffer.Size;
		idxDst += cmdList->IdxBuffer.Size;
	}

	// RenderState
	SR_ImGuiVertexConstants vertexConstants;
	{
		float L = drawData->DisplayPos.x;
		float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
		float T = drawData->DisplayPos.y;	   
		float B = drawData->DisplayPos.y + drawData->DisplaySize.y;
		vertexConstants.mMVP = SC_Matrix(
			2.0f / (R - L), 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / (T - B), 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f
		);
		vertexConstants.mTextureIndex = mFontTexture->GetDescriptorHeapIndex();
		vertexConstants.mDPIScale = mDPIScale;
	}

	mCommandList->TransitionBarrier(SR_ResourceState_RenderTarget, aRenderTarget->GetResource());
	mCommandList->SetRenderTargets(1, &aRenderTarget, nullptr);

	SR_Rect r = { 0, 0, (uint32)drawData->DisplaySize.x, (uint32)drawData->DisplaySize.y };

	mCommandList->SetViewport(r);
	mCommandList->SetScissorRect(r);
	mCommandList->SetPrimitiveTopology(SR_PrimitiveTopology::TriangleList);
	mCommandList->SetVertexBuffer(renderBuffer.mVertexBuffer.get());
	mCommandList->SetIndexBuffer(renderBuffer.mIndexBuffer.get());
	mCommandList->SetShaderState(mShaderState.get());
	mCommandList->SetBlendFactor(SC_Vector4(0.0f));

	int globalVtxOffset = 0;
	int globalIdxOffset = 0;
	ImVec2 clipOffset = drawData->DisplayPos;
	uint32 i = 0;
	int cb = 0;
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmdList = drawData->CmdLists[n];
		for (const ImDrawCmd& cmd : cmdList->CmdBuffer)
		{
			if (cmd.UserCallback != nullptr)
			{
				if (cmd.UserCallback == ImDrawCallback_ResetRenderState)
				{
					mCommandList->SetViewport(SR_Rect{ 0, 0, (uint32)drawData->DisplaySize.x, (uint32)drawData->DisplaySize.y });
					mCommandList->SetPrimitiveTopology(SR_PrimitiveTopology::TriangleList);
					mCommandList->SetVertexBuffer(renderBuffer.mVertexBuffer.get());
					mCommandList->SetIndexBuffer(renderBuffer.mIndexBuffer.get());
					mCommandList->SetShaderState(mShaderState.get());
					mCommandList->SetBlendFactor(SC_Vector4(0.0f));
				}
				else
					cmd.UserCallback(cmdList, &cmd);
			}
			else
			{
				// Apply Scissor, Bind texture, Draw
				const SC_Vector2 clipMin(cmd.ClipRect.x - clipOffset.x, cmd.ClipRect.y - clipOffset.y);
				const SC_Vector2 clipMax(cmd.ClipRect.z - clipOffset.x, cmd.ClipRect.w - clipOffset.y);
				if (clipMax.x < clipMin.x || clipMax.y < clipMin.y)
					continue;

				SR_Texture* texture = (SR_Texture*)cmd.TextureId; // use this for accessing through bindless
				if (texture)
					vertexConstants.mTextureIndex = texture->GetDescriptorHeapIndex();
				else
					vertexConstants.mTextureIndex = mFontTexture->GetDescriptorHeapIndex();

				SR_BufferResourceProperties cbDesc;
				cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
				cbDesc.mElementCount = 1;
				cbDesc.mElementSize = sizeof(SR_ImGuiVertexConstants);
				if (cb > (mConstantBuffers.ICount() - 1))
					mConstantBuffers.Add(SR_RenderDevice::gInstance->CreateBufferResource(cbDesc));
				else if (!mConstantBuffers[cb])
					mConstantBuffers[cb] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);

				mConstantBuffers[cb]->UpdateData(0, &vertexConstants, sizeof(SR_ImGuiVertexConstants));
				mCommandList->SetRootConstantBuffer(mConstantBuffers[cb].get(), 0);

				const SR_Rect scissorRect = { (uint32)clipMin.x, (uint32)clipMin.y, (uint32)clipMax.x, (uint32)clipMax.y };
				mCommandList->SetScissorRect(scissorRect);

				mCommandList->DrawIndexedInstanced(cmd.ElemCount, 1, cmd.IdxOffset + globalIdxOffset, cmd.VtxOffset + globalVtxOffset, 0);
				++cb;
			}
			++i;
		}
		globalVtxOffset += cmdList->VtxBuffer.Size;
		globalIdxOffset += cmdList->IdxBuffer.Size;
	}
	mCommandList->TransitionBarrier(SR_ResourceState_Present, aRenderTarget->GetResource());
	mCommandList->End();

	SR_CommandQueue* cmdQueue = SR_RenderDevice::gInstance->GetCommandQueue(SR_CommandListType::Graphics);
	mLastFence = cmdQueue->SubmitCommandList(mCommandList.get(), "Render ImGui");
}

void SR_ImGui::SetDPIScale(float aScale)
{
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = aScale;
	mDPIScale = aScale;
}

SR_CommandList* SR_ImGui::GetCommandList() const
{
	return mCommandList.get();
}

bool SR_ImGui::InitRenderObjects()
{
	if (!CreateShaderState())
		return false;

	if (!CreateFontTexture())
		return false;

	mCommandList = SR_RenderDevice::gInstance->CreateCommandList(SR_CommandListType::Graphics);
	return true;
}

bool SR_ImGui::InitPlatform(void* aNativeWindowHandle)
{
#if IS_WINDOWS_PLATFORM
	return InitPlatformWin64(aNativeWindowHandle);
#else
#error Platform not supported yet!
#endif
}

#if IS_WINDOWS_PLATFORM

void SR_ImGui::NewFrameWin64()
{
	ImGuiIO& io = ImGui::GetIO(); 
	IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built!");

	RECT rect = { 0, 0, 0, 0 };
	::GetClientRect((HWND)mNativeWindowHandle, &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
	//if (g_WantUpdateMonitors)
	//	ImGui_ImplWin32_UpdateMonitors();

	int64 currentTime = 0;
	::QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	io.DeltaTime = (float)(currentTime - mTime) / mTicksPerSecond;
	mTime = currentTime;

	UpdateMousePos();

	ImGuiMouseCursor mouseCursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
	if (mLastCursor != mouseCursor)
	{
		mLastCursor = mouseCursor;
		SR_ImGui_UpdateMouseCursor();
	}

	// Update Gamepads
}

bool SR_ImGui::InitPlatformWin64(void* aNativeWindowHandle)
{
	if (!::QueryPerformanceFrequency((LARGE_INTEGER*)&mTicksPerSecond))
		return false;
	if (!::QueryPerformanceCounter((LARGE_INTEGER*)&mTime))
		return false;

	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can set io.MouseHoveredViewport correctly (optional, not easy)
	io.BackendPlatformName = "SR_ImGui_Win64";

	mNativeWindowHandle = aNativeWindowHandle;
	ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	mainViewport->PlatformHandle = mainViewport->PlatformHandleRaw = mNativeWindowHandle;
	//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		//ImGui_ImplWin32_InitPlatformInterface();


	if (!mInputHandler.Init())
		return false;

	return true;
}
#endif

bool SR_ImGui::CreateRootSignature()
{
	return false;
}

bool SR_ImGui::CreateShaderState()
{
	SR_ShaderStateProperties shaderProps;

	SR_ShaderCompileArgs compileArgs;
	compileArgs.mEntryPoint = "main";
	compileArgs.mType = SR_ShaderType::Vertex;
	if (!SR_RenderDevice::gInstance->CompileShader(GetVertexShaderCode(), compileArgs, shaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Vertex)]))
	{
		return false;
	}

	compileArgs.mType = SR_ShaderType::Pixel;
	if (!SR_RenderDevice::gInstance->CompileShader(GetPixelShaderCode(), compileArgs, shaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Pixel)]))
	{
		return false;
	}

	shaderProps.mVertexLayout.SetAttribute(SR_VertexAttribute::Position, SR_Format::RG32_FLOAT);
	shaderProps.mVertexLayout.SetAttribute(SR_VertexAttribute::UV0, SR_Format::RG32_FLOAT);
	shaderProps.mVertexLayout.SetAttribute(SR_VertexAttribute::Color0, SR_Format::RGBA8_UNORM);

	shaderProps.mRTVFormats.mNumColorFormats = 1;
	shaderProps.mRTVFormats.mColorFormats[0] = SR_Format::RGBA8_UNORM;

	shaderProps.mRasterizerProperties.mCullMode = SR_CullMode::None;

	shaderProps.mBlendStateProperties.mAlphaToCoverage = false;
	shaderProps.mBlendStateProperties.mNumRenderTargets = 1;
	shaderProps.mBlendStateProperties.mRenderTagetBlendProperties[0].mEnableBlend = true;
	shaderProps.mBlendStateProperties.mRenderTagetBlendProperties[0].mSrcBlend = SR_BlendMode::SrcAlpha;
	shaderProps.mBlendStateProperties.mRenderTagetBlendProperties[0].mDstBlend = SR_BlendMode::OneMinusSrcAlpha;
	shaderProps.mBlendStateProperties.mRenderTagetBlendProperties[0].mBlendFunc = SR_BlendFunc::Add;
	shaderProps.mBlendStateProperties.mRenderTagetBlendProperties[0].mSrcBlendAlpha = SR_BlendMode::OneMinusSrcAlpha;
	shaderProps.mBlendStateProperties.mRenderTagetBlendProperties[0].mDstBlendAlpha = SR_BlendMode::Zero;
	shaderProps.mBlendStateProperties.mRenderTagetBlendProperties[0].mBlendFuncAlpha = SR_BlendFunc::Add;
	shaderProps.mBlendStateProperties.mRenderTagetBlendProperties[0].mWriteMask = static_cast<uint8>(SR_ColorWriteMask::RGBA);

	SR_RootSignatureProperties rootSignatureProperties;

	SR_RootParam& rootParam = rootSignatureProperties.mRootParams.Add();
	rootParam.InitAsDescriptor(0, 0, SR_RootParamVisibility::All, SR_RootParamType::CBV);

	SR_StaticSamplerProperties& rootSampler = rootSignatureProperties.mStaticSamplers.Add();
	rootSampler.mProperties.mBorderColor = SC_Vector4(0.0f);
	rootSampler.mProperties.mComparison = SR_ComparisonFunc::Always;
	rootSampler.mProperties.mLODBias = 0.0f;
	rootSampler.mProperties.mMinFilter = SR_FilterMode::Linear;
	rootSampler.mProperties.mMagFilter = SR_FilterMode::Linear;
	rootSampler.mProperties.mMipFilter = SR_FilterMode::Linear;
	rootSampler.mProperties.mWrapX = SR_WrapMode::Wrap;
	rootSampler.mProperties.mWrapY = SR_WrapMode::Wrap;
	rootSampler.mProperties.mWrapZ = SR_WrapMode::Wrap;
	rootSampler.mProperties.mMaxAnisotropy = 0;
	rootSampler.mVisibility = SR_RootParamVisibility::Pixel;

	rootSignatureProperties.mFlags = SR_RootSignatureFlag_DefaultSetup | SR_RootSignatureFlag_DescriptorHeapDirectAccess;

#if ENABLE_DX12
	SC_Ref<SR_RootSignature_DX12> rootSigDX12 = SC_MakeRef<SR_RootSignature_DX12>(rootSignatureProperties);
	if (!rootSigDX12->Init())
		return false;

	mRootSignature = rootSigDX12;
#else
//#error Platform not supported yet!
#endif

	shaderProps.mRootSignature = mRootSignature;
	mShaderState = SR_RenderDevice::gInstance->CreateShaderState(shaderProps);

	return true;
}

void SR_ImGui::UpdateMousePos()
{
	ImGuiIO& io = ImGui::GetIO(); 
	HWND hwnd = (HWND)mNativeWindowHandle;

	if (io.WantSetMousePos)
	{
		POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
		if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0 || ::ClientToScreen(hwnd, &pos))
		{
			::ClientToScreen(hwnd, &pos);
		}
		::SetCursorPos(pos.x, pos.y);
	}

	io.MousePos = ImVec2(SC_FLT_LOWEST, SC_FLT_LOWEST);
	io.MouseHoveredViewport = 0;

	POINT mouseScreenPos;
	if (!::GetCursorPos(&mouseScreenPos))
		return;

	if (HWND focusedHwnd = ::GetForegroundWindow())
	{
		if (::IsChild(focusedHwnd, hwnd))
			focusedHwnd = hwnd;

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			if (ImGui::FindViewportByPlatformHandle((void*)focusedHwnd) != NULL)
				io.MousePos = ImVec2((float)mouseScreenPos.x, (float)mouseScreenPos.y);
		}
		else if (focusedHwnd == hwnd)
		{
			POINT mouseClientPos = mouseScreenPos;
			::ScreenToClient(focusedHwnd, &mouseClientPos);
			io.MousePos = ImVec2((float)mouseClientPos.x, (float)mouseClientPos.y);
		}
	}

	if (HWND hoveredHwnd = ::WindowFromPoint(mouseScreenPos))
	{
		if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle((void*)hoveredHwnd))
		{
			if ((viewport->Flags & ImGuiViewportFlags_NoInputs) == 0)
				io.MouseHoveredViewport = viewport->ID;
		}
	}
}

bool SR_ImGui::CreateFontTexture()
{
	ImGuiIO& io = ImGui::GetIO();
	uint8* pixels = nullptr;
	int width = 0;
	int height = 0;
	int bytesPerPixel = 0;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytesPerPixel);

	SR_PixelData pixelData;
	pixelData.mBytesPerLine = width * bytesPerPixel;
	pixelData.mBytesPerSlice = pixelData.mBytesPerLine * height;
	pixelData.mSize = width * height * bytesPerPixel;
	pixelData.mData = pixels;

	SR_TextureResourceProperties fontResourceProps;
	fontResourceProps.mFormat = SR_Format::RGBA8_UNORM;
	fontResourceProps.mSize = SC_IntVector(width, height, 1);
	fontResourceProps.mNumMips = 1;
	fontResourceProps.mType = SR_ResourceType::Texture2D;
	fontResourceProps.mDebugName = "ImGui/Fonts";
	SC_Ref<SR_TextureResource> fontsResource = SR_RenderDevice::gInstance->CreateTextureResource(fontResourceProps, &pixelData, 1);

	SR_TextureProperties fontTextureProps;
	fontTextureProps.mDimension = SR_TextureDimension::Texture2D;
	fontTextureProps.mFirstArrayIndex = 0;
	fontTextureProps.mFormat = SR_Format::RGBA8_UNORM;
	fontTextureProps.mWritable = false;
	mFontTexture = SR_RenderDevice::gInstance->CreateTexture(fontTextureProps, fontsResource);

	return (mFontTexture != nullptr);
}

void SR_ImGui::SetStyle()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.10f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.24f, 0.25f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.39f, 0.39f, 0.41f, 0.78f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.39f, 0.39f, 0.41f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.24f, 0.24f, 0.25f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.12f, 0.14f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.33f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.65f, 1.00f, 0.78f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.47f, 0.49f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.65f, 0.94f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.65f, 0.94f, 0.78f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.65f, 0.94f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.24f, 0.24f, 0.25f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.65f, 0.94f, 0.78f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.65f, 0.94f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.18f, 0.18f, 0.18f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 0.65f, 0.94f, 0.78f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 0.64f, 0.94f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.00f, 0.65f, 0.94f, 0.78f);
	colors[ImGuiCol_TabActive] = ImVec4(0.00f, 0.44f, 0.65f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.00f, 0.65f, 0.94f, 0.71f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

}

void SR_ImGui::Shutdown()
{
	ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	if (SR_ImGuiViewportData* data = (SR_ImGuiViewportData*)mainViewport->RendererUserData)
	{
		IM_DELETE(data);
		mainViewport->RendererUserData = nullptr;
	}

#if IS_WINDOWS_PLATFORM
	ImGui::DestroyPlatformWindows();
#else
#error Platform not supported.
#endif
}

SR_ImGui* SR_ImGui::Get()
{
	return gInstance;
}

bool SR_ImGui::Input::Init()
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = static_cast<int32>(SC_KeyCode::Tab);
	io.KeyMap[ImGuiKey_LeftArrow] = static_cast<int32>(SC_KeyCode::LeftArrow);
	io.KeyMap[ImGuiKey_RightArrow] = static_cast<int32>(SC_KeyCode::RightArrow);
	io.KeyMap[ImGuiKey_UpArrow] = static_cast<int32>(SC_KeyCode::UpArrow);
	io.KeyMap[ImGuiKey_PageUp] = static_cast<int32>(SC_KeyCode::PageUp);
	io.KeyMap[ImGuiKey_PageDown] = static_cast<int32>(SC_KeyCode::PageDown);
	io.KeyMap[ImGuiKey_Home] = static_cast<int32>(SC_KeyCode::Home);
	io.KeyMap[ImGuiKey_End] = static_cast<int32>(SC_KeyCode::End);
	io.KeyMap[ImGuiKey_Insert] = static_cast<int32>(SC_KeyCode::Insert);
	io.KeyMap[ImGuiKey_Delete] = static_cast<int32>(SC_KeyCode::Delete);
	io.KeyMap[ImGuiKey_Backspace] = static_cast<int32>(SC_KeyCode::Backspace);
	io.KeyMap[ImGuiKey_Space] = static_cast<int32>(SC_KeyCode::Space);
	io.KeyMap[ImGuiKey_Enter] = static_cast<int32>(SC_KeyCode::Enter);
	io.KeyMap[ImGuiKey_Escape] = static_cast<int32>(SC_KeyCode::Escape);
	//io.KeyMap[ImGuiKey_KeyPadEnter] = static_cast<int32>(SC_KeyCode::numpa);
	io.KeyMap[ImGuiKey_A] = static_cast<int32>(SC_KeyCode::A);
	io.KeyMap[ImGuiKey_C] = static_cast<int32>(SC_KeyCode::C);
	io.KeyMap[ImGuiKey_V] = static_cast<int32>(SC_KeyCode::V);
	io.KeyMap[ImGuiKey_X] = static_cast<int32>(SC_KeyCode::X);
	io.KeyMap[ImGuiKey_Y] = static_cast<int32>(SC_KeyCode::Y);
	io.KeyMap[ImGuiKey_Z] = static_cast<int32>(SC_KeyCode::Z);
	return true;
}

void SR_ImGui::Input::RecieveMessage(const SC_Message& aMsg)
{
	switch (aMsg.mType)
	{
	case SC_MessageType::InputChar:
	{
		AddInputChar(aMsg.Get<uint16>());
		break;
	}
	case SC_MessageType::Key:
	{
		SC_KeyMessageData msgData(aMsg.Get<SC_KeyMessageData>());
		if (msgData.mState == SC_InputKeyStateMessage::Pressed)
			ButtonPress(msgData.mKey);
		else if (msgData.mState == SC_InputKeyStateMessage::Released)
			ButtonRelease(msgData.mKey);
		break;
	}
	case SC_MessageType::Mouse:
	{
		SC_MouseKeyMessageData msgData(aMsg.Get<SC_MouseKeyMessageData>()); 
		if (msgData.mState == SC_InputKeyStateMessage::Pressed)
			MousePress(msgData.mKey);
		else if (msgData.mState == SC_InputKeyStateMessage::Released)
			MouseRelease(msgData.mKey);
		break;
		break;
	}
	case SC_MessageType::MousePos:
	{
		uint32 msgData = aMsg.Get<uint32>();
		int16 x = static_cast<int16>(msgData & 0x0000ffff);
		int16 y = static_cast<int16>(msgData >> 16);
		SetMousePos(x, y);
		break;
	}
	case SC_MessageType::Scroll:
	{
		SetScroll(aMsg.Get<float>());
		break;
	}
	}
}

void SR_ImGui::Input::AddInputChar(uint16 aCharacter)
{
	if (!ImGui::GetCurrentContext())
		return;

	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(aCharacter);
}

void SR_ImGui::Input::ButtonPress(const SC_KeyCode& aKey)
{
	if (!ImGui::GetCurrentContext())
		return;

	ImGuiIO& io = ImGui::GetIO();

	if (aKey == SC_KeyCode::Control)
		io.KeyCtrl = true;
	else if (aKey == SC_KeyCode::Shift)
		io.KeyShift = true;
	else if (aKey == SC_KeyCode::LeftAlt || aKey == SC_KeyCode::RightAlt)
		io.KeyAlt = true;

	io.KeysDown[static_cast<uint32>(aKey)] = true;
}

void SR_ImGui::Input::ButtonRelease(const SC_KeyCode& aKey)
{
	if (!ImGui::GetCurrentContext())
		return;

	ImGuiIO& io = ImGui::GetIO();

	if (aKey == SC_KeyCode::Control)
		io.KeyCtrl = false;
	else if (aKey == SC_KeyCode::Shift)
		io.KeyShift = false;
	else if (aKey == SC_KeyCode::LeftAlt || aKey == SC_KeyCode::RightAlt)
		io.KeyAlt = false;

	io.KeysDown[static_cast<uint32>(aKey)] = false;
}

void SR_ImGui::Input::MousePress(const SC_MouseKeyCode& aMouseKey)
{
	if (!ImGui::GetCurrentContext())
		return;

	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[static_cast<uint32>(aMouseKey)] = true;
}

void SR_ImGui::Input::MouseRelease(const SC_MouseKeyCode& aMouseKey)
{
	if (!ImGui::GetCurrentContext())
		return;

	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[static_cast<uint32>(aMouseKey)] = false;
}

void SR_ImGui::Input::SetMousePos(int16 aX, int16 aY)
{
	if (!ImGui::GetCurrentContext())
		return;

	ImGuiIO& io = ImGui::GetIO();
	io.MousePos.x = aX;
	io.MousePos.y = aY;
}

void SR_ImGui::Input::SetScroll(float aScrollDelta)
{
	if (!ImGui::GetCurrentContext())
		return;

	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheel += aScrollDelta;
}
