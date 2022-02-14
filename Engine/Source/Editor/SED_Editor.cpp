#include "SED_Editor.h"

#include "SED_WorldHierarchyPanel.h"
#include "SED_PropertiesPanel.h"
#include "SED_ContentBrowserPanel.h"
#include "SED_ViewportPanel.h"
#include "SED_MaterialEditor.h"
#include "SED_TransformationGizmo.h"

#include "GameFramework/Entity/SGF_ECSModule.h"
#include "GameFramework/GameWorld/SGF_World.h"
#include "GameFramework/Entity/Components/SGF_TransformComponent.h"
#include "GameFramework/Entity/SGF_Entity.h"

#include "Graphics/World/SGfx_World.h"
#include "Graphics/View/SGfx_View.h"
#include "Graphics/Renderer/SGfx_Renderer.h"
#include "Graphics/Lighting/SGfx_AmbientOcclusion.h"
#include "Graphics/Lighting/Shadows/SGfx_ShadowSystem.h"

#include "Platform/Time/SC_Time.h"
#include "RenderCore/Interface/SR_RenderDevice.h"

SED_Editor::SED_Editor()
	: mSelectedEntity(nullptr)
	, mIsDemoWindowOpen(false)
	, mIsMetricsWindowOpen(false)
{

}

SED_Editor::~SED_Editor()
{

}

bool SED_Editor::Init()
{
	if (!mImGui.Init(SAF_Framework::Get()->GetNativeWindowHandle(), SAF_Framework::Get()->GetWindowDPI()))
		return false;

	SGF_ECSModule::RegisterComponents();

	mActiveWorld = SC_MakeRef<SGF_World>();
	mActiveWorld->Init();

	mWorldHierarchy = SC_MakeRef<SED_WorldHierarchyPanel>(mActiveWorld);
	mPropertiesPanel = SC_MakeRef<SED_PropertiesPanel>();
	mViewport = SC_MakeRef<SED_ViewportPanel>(mActiveWorld->GetGraphicsWorld(), &mGizmo);

	mPanels.Add(mWorldHierarchy);
	mPanels.Add(mPropertiesPanel);
	mPanels.Add(mViewport);
	mPanels.Add(SC_MakeRef<SED_ContentBrowserPanel>());

	auto WorldLoadingTask = [&]()
	{
		mActiveWorld->LoadLevel("");
	};
	SC_ThreadPool::Get().SubmitTask(WorldLoadingTask);

	//mMaterialEditor = SC_MakeUnique<SED_MaterialEditor>();
	//if (!mMaterialEditor->Init())
	//	return false;

	return true;
}

bool SED_Editor::Update()
{
	mActiveWorld->Update();

	for (auto& panel : mPanels)
		panel->Update();

	//mMaterialEditor->OnUpdate();

	mSelectedEntity = mWorldHierarchy->GetSelected();
	mPropertiesPanel->SetSelectedEntity(mSelectedEntity);
	return true;
}

bool SED_Editor::Render()
{
	mImGui.BeginFrame();
	mGizmo.BeginFrame(mViewport->GetViewportBounds());

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	bool dockspaceOpen = true;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("MainWindow", &dockspaceOpen, windowFlags);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);

	ImGuiID dockspaceId = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	if (ImGui::BeginMainMenuBar())
	{
		//if (ImGui::BeginMenu("File"))
		//{
		//	ImGui::EndMenu();
		//}

		if (ImGui::BeginMenu("ImGui"))
		{
			if (ImGui::MenuItem("Demo Window"))
				mIsDemoWindowOpen = true;
			if (ImGui::MenuItem("Metrics Window"))
				mIsMetricsWindowOpen = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Renderer"))
		{
			SGfx_Renderer* renderer = mActiveWorld->GetGraphicsWorld()->GetRenderer();
			SGfx_Renderer::Settings& rendererSettings = renderer->GetSettings();
			ImGui::Checkbox("Enable TAA", &rendererSettings.mEnableTemporalAA);
			ImGui::Checkbox("Draw Grid", &rendererSettings.mDrawGridHelper);

			if (ImGui::BeginMenu("Ambient Occlusion"))
			{
				SGfx_AmbientOcclusion* ao = renderer->GetAmbientOcclusion();
				SGfx_AmbientOcclusion::RTAOSettings& aoSettings = ao->GetRTAOSettings();
				const char* types[] = { "Disabled", "GTAO", "RTAO" };

				uint32 currentType = static_cast<uint32>(aoSettings.mAOType);
				if (ImGui::BeginCombo("Type", types[currentType]))
				{
					for (uint32 i = 0; i < 3; ++i)
					{
						bool selected = currentType == i;
						if (ImGui::Selectable(types[i], &selected))
						{
							aoSettings.mAOType = (SGfx_AmbientOcclusion::Type)i;
						}
					}

					ImGui::EndCombo();
				}

				if (aoSettings.mAOType == SGfx_AmbientOcclusion::Type::RTAO)
				{
					ImGui::SliderInt("Num Rays", &aoSettings.mNumRaysPerPixel, 0, 8);
					ImGui::SliderFloat("Radius", &aoSettings.mRadius, 0.1f, 25.0f);
					ImGui::Checkbox("Denoise", &aoSettings.mUseDenoiser);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Shadows"))
			{
				SGfx_CascadedShadowMap::Settings& csmSettings = renderer->GetShadowMapSystem()->GetCSM()->GetSettings();
				const char* resolutions[] = { "512", "1024", "2048", "4096" };

				uint32 resolutionIndex = 0;
				switch (csmSettings.mResolution)
				{
				case 512:
					resolutionIndex = 0;
					break;
				case 1024:
					resolutionIndex = 1;
					break;
				case 2048:
					resolutionIndex = 2;
					break;
				case 4096:
					resolutionIndex = 3;
					break;
				}

				if (ImGui::BeginCombo("CSM Resolution", resolutions[static_cast<uint32>(resolutionIndex)]))
				{
					for (uint32 i = 0; i < 4; ++i)
					{
						bool isSelected = resolutionIndex == i;
						if (ImGui::Selectable(resolutions[i], &isSelected))
						{
							switch (i)
							{
							case 0:
								csmSettings.mResolution = 512;
								break;
							case 1:
								csmSettings.mResolution = 1024;
								break;
							case 2:
								csmSettings.mResolution = 2048;
								break;
							case 3:
								csmSettings.mResolution = 4096;
								break;
							}
						}
					}
					ImGui::EndCombo();
				}


				ImGui::SliderFloat("CSM Max Distance", &csmSettings.mMaxDistance, 1.0f, 1000.0f, "%.0f");
				ImGui::SliderFloat("CSM Split Factor", &csmSettings.mSplitFactor, 0.7f, 1.0f, "%.2f");

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (mIsDemoWindowOpen)
		ImGui::ShowDemoWindow(&mIsDemoWindowOpen);
	if (mIsMetricsWindowOpen)
		ImGui::ShowMetricsWindow(&mIsMetricsWindowOpen);

	for (auto& panel : mPanels)
		panel->OnRender();

	//mMaterialEditor->OnRender();

	if (mSelectedEntity)
	{
		SGF_TransformComponent* transformComp = mSelectedEntity->GetComponent<SGF_TransformComponent>();
		if (transformComp)
		{
			const SGfx_ViewConstants constants = mViewport->GetCamera().GetViewConstants();
			const SC_Vector4 viewportBounds = mViewport->GetViewportBounds();

			mGizmo.SetViewportPositionAndSize(SC_Vector4(viewportBounds.x, viewportBounds.y, viewportBounds.z - viewportBounds.x, viewportBounds.w - viewportBounds.y));
			mGizmo.SetViewAndProjection(constants.mWorldToCamera, constants.mCameraToClip);

			SC_Matrix transform = transformComp->GetTransform();
			mGizmo.Manipulate(transform);
		}
	}

	ImGui::End();

	mImGui.Render(SR_RenderDevice::gInstance->GetSwapChain()->GetRenderTarget());
	return true;
}

void SED_Editor::Exit()
{

}
