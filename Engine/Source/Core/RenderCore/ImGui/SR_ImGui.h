#pragma once
#include "RenderCore/Interface/SR_Fence.h"
#include "Common/MessageQueue/SC_MessageListener.h"
#include "InputOutput/KeyCodes/SC_KeyCodes.h"

struct ID3D12RootSignature;

class SR_ShaderState;
class SR_BufferResource;
class SR_RootSignature;
class SR_CommandList;
class SR_Texture;
class SR_RenderTarget;

struct SC_InputMessage;

class SR_ImGui
{
public:
	SR_ImGui();
	~SR_ImGui();

	bool Init(void* aNativeWindowHandle, float aDPIScale);
	void BeginFrame();
	void Render(SR_RenderTarget* aRenderTarget);

	void SetDPIScale(float aScale);

	struct Input : public SC_MessageListener
	{
	public:
		bool Init();
	protected:
		void RecieveMessage(const SC_Message& aMsg) override;
	private:
		void AddInputChar(uint16 aCharacter);
		void ButtonPress(const SC_KeyCode& aKey);
		void ButtonRelease(const SC_KeyCode& aKey);
		void MousePress(const SC_MouseKeyCode& aMouseKey);
		void MouseRelease(const SC_MouseKeyCode& aMouseKey);
		void SetMousePos(int16 aX, int16 aY);
		void SetScroll(float aScrollDelta);
	};

	static SR_ImGui* Get();

private:
	bool InitRenderObjects();
	bool InitPlatform(void* aNativeWindowHandle);

#if IS_WINDOWS_PLATFORM
	void NewFrameWin64();
	bool InitPlatformWin64(void* aNativeWindowHandle);
	bool InitPlatformInterfaceWin64();
#endif

	bool CreateRootSignature();
	bool CreateShaderState();
	void UpdateMousePos();
	bool CreateFontTexture();

	void SetStyle();

	void Shutdown();

	Input mInputHandler;

	SC_Ref<SR_RootSignature> mRootSignature;
	SC_Ref<SR_ShaderState> mShaderState;
	SC_Ref<SR_TaskEvent> mLastTaskEvent;
	SC_Ref<SR_Texture> mFontTexture;

	SR_Fence mLastFence;
	int64 mTicksPerSecond;
	int64 mTime;
	void* mNativeWindowHandle;
	int mLastCursor;

	float mDPIScale;

	static SR_ImGui* gInstance;
};

