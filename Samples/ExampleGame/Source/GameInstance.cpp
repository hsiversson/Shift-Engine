#include "GameInstance.h"

#include "GameFramework/GameWorld/SGF_World.h"
#include "GameFramework/Entity/SGF_ECSModule.h"

#include "Graphics/World/SGfx_World.h"
#include "RenderCore/Interface/SR_CommandList.h"
#include "Graphics/View/SGfx_View.h"
#include "RenderCore/Interface/SR_RenderDevice.h"
#include "Graphics/Renderer/SGfx_Renderer.h"

GameInstance::GameInstance()
{

}

GameInstance::~GameInstance()
{

}

bool GameInstance::Init()
{
	SGF_ECSModule::RegisterComponents();

	mActiveWorld = SC_MakeRef<SGF_World>();
	mActiveWorld->Init();

	mView = mActiveWorld->GetGraphicsWorld()->CreateView();
	mView->SetMainView(true);

	mCamera.SetPerspectiveProjection(SR_RenderDevice::gInstance->GetSwapChain()->GetProperties().mSize, 0.01f, 10000.f, 90.0f);
	mCamera.SetPosition({ -10.0f, 2.0f, 0.0f });
	mCamera.LookAt({ 0.f, 1.f, 0.f });
	mView->SetCamera(mCamera);

	mActiveWorld->LoadLevel("");
	return true;
}

bool GameInstance::Update()
{
	SC_PROFILER_FUNCTION();
	mActiveWorld->Update();
	mActiveWorld->GetGraphicsWorld()->PrepareView(mView.Get());
	mActiveWorld->GetGraphicsWorld()->RenderView(mView.Get());
	return true;
}

bool GameInstance::Render()
{
	SC_PROFILER_FUNCTION();

	auto CopyToBackBuffer = [this]()
	{
		SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
		cmdList->TransitionBarrier(SR_ResourceState_CopySrc, mActiveWorld->GetGraphicsWorld()->GetRenderer()->GetScreenColor()->GetResource());
		cmdList->CopyResource(SR_RenderDevice::gInstance->GetSwapChain()->GetTexture()->GetResource(), mActiveWorld->GetGraphicsWorld()->GetRenderer()->GetScreenColor()->GetResource());
		cmdList->TransitionBarrier(SR_ResourceState_Present, mActiveWorld->GetGraphicsWorld()->GetRenderer()->GetScreenColor()->GetResource());
	};
	copyEvent = SR_RenderDevice::gInstance->PostGraphicsTask(CopyToBackBuffer);

	return true;
}

void GameInstance::Exit()
{
	mActiveWorld->GetGraphicsWorld()->DestroyView(mView);
}

