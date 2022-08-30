#include "SED_Editor.h"

#include "SED_WorldHierarchyWindow.h"
#include "SED_PropertiesWindow.h"
#include "SED_ContentBrowserWindow.h"
#include "SED_ViewportWindow.h"
#include "SED_MaterialEditor.h"
#include "SED_TransformationGizmo.h"
#include "SED_Icons.h"

#include "GameFramework/Entity/SGF_ECSModule.h"
#include "GameFramework/GameWorld/SGF_World.h"
#include "GameFramework/Entity/Components/SGF_TransformComponent.h"
#include "GameFramework/Entity/SGF_Entity.h"

#include "Graphics/World/SGfx_World.h"
#include "Graphics/View/SGfx_View.h"
#include "Graphics/Renderer/SGfx_Renderer.h"
#include "Graphics/Lighting/SGfx_AmbientOcclusion.h"
#include "Graphics/Lighting/Shadows/SGfx_ShadowSystem.h"
#include "Graphics/Lighting/Raytracing/SGfx_Raytracing.h"

#include "Platform/Time/SC_Time.h"
#include "RenderCore/Interface/SR_RenderDevice.h"
#include "SED_MetricsWindow.h"
#include "SED_Widgets.h"

class SED_GraphicsSettingsWindow : public SED_Window
{
public:
	SED_GraphicsSettingsWindow() : mIsFullscreen(false) {}
	~SED_GraphicsSettingsWindow() {}

	const char* GetWindowName() const override { return "Graphics Settings"; }

protected:
	void OnUpdate() override
	{

	}

	void OnDraw() override
	{
		if (SED_Checkbox("Fullscreen", mIsFullscreen))
		{
			SAF_Framework::Get()->SetFullscreen(mIsFullscreen);
		}
	}

private:
	bool mIsFullscreen;
};


SED_Editor::SED_Editor()
	: mIsDemoWindowOpen(false)
{
}

SED_Editor::~SED_Editor()
{
	SED_Icons::Destroy();
}

bool SED_Editor::Init()
{
	SED_Icons::Create();

	if (!mImGui.Init(SAF_Framework::Get()->GetNativeWindowHandle(), SAF_Framework::Get()->GetWindowDPI()))
		return false;

	SGF_ECSModule::RegisterComponents();

	mActiveWorld = SC_MakeRef<SGF_World>();
	mActiveWorld->Init();

	mWorldHierarchy = SC_MakeRef<SED_WorldHierarchyWindow>(mActiveWorld);
	mPropertiesPanel = SC_MakeRef<SED_PropertiesWindow>(mActiveWorld);
	mViewport = SC_MakeRef<SED_ViewportWindow>(mActiveWorld->GetGraphicsWorld());

	mWindows.Add(mWorldHierarchy);
	mWindows.Add(mPropertiesPanel);
	mWindows.Add(mViewport);
	mWindows.Add(SC_MakeRef<SED_ContentBrowserWindow>());

	mMetricsWindow = SC_MakeRef<SED_MetricsWindow>();
	mWindows.Add(mMetricsWindow);

	mGraphicsSettings = SC_MakeRef<SED_GraphicsSettingsWindow>();
	mWindows.Add(mGraphicsSettings);

	//auto WorldLoadingTask = [&]()
	//{
		mActiveWorld->LoadLevel("");
	//};
	//SC_ThreadPool::Get().SubmitTask(WorldLoadingTask);

	//mMaterialEditor = SC_MakeUnique<SED_MaterialEditor>();
	//if (!mMaterialEditor->Init())
	//	return false;

	return true;
}

bool SED_Editor::Update()
{
	SC_PROFILER_FUNCTION();
	mActiveWorld->Update();

	//SC_Array<SC_Future<bool>> taskEvents;

	for (auto& panel : mWindows)
	{
		panel->Update();
		//taskEvents.Add(SC_ThreadPool::Get().SubmitTask([panel]() { panel->Update(); }));
	}

	//for (SC_Future<bool>& taskEvent : taskEvents)
	//	taskEvent.Wait();

	//mMaterialEditor->OnUpdate();

	mSelectedEntity = mWorldHierarchy->GetSelected();
	mPropertiesPanel->SetSelectedEntity(mSelectedEntity);
	mViewport->SetSelectedEntity(mSelectedEntity);
	return true;
}

bool SED_Editor::Render()
{
	SC_PROFILER_FUNCTION();
	mImGui.BeginFrame();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;

	bool dockspaceOpen = true;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("MainWindow", &dockspaceOpen, windowFlags);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);

	ImGuiID dockspaceId = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	if (mIsDemoWindowOpen)
		ImGui::ShowDemoWindow(&mIsDemoWindowOpen);

	if (ImGui::BeginMenuBar())
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
				mMetricsWindow->Open();
			if (ImGui::MenuItem("Graphics Settings"))
				mMetricsWindow->Open();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Renderer"))
		{
			SGfx_Renderer* renderer = mActiveWorld->GetGraphicsWorld()->GetRenderer();
			SGfx_DDGI* rtgi = renderer->GetRTGI();
			SGfx_DDGI::ProbeGridProperties& probeGridProperties = rtgi->mProbeGridProperties;
			SGfx_Renderer::Settings& rendererSettings = renderer->GetSettings();
			SED_Checkbox("Enable TAA", rendererSettings.mEnableTemporalAA);

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
					SED_IntSlider("Num Rays", aoSettings.mNumRaysPerPixel, 0, 8);
					SED_FloatSlider("Radius", aoSettings.mRadius, 0.1f, 25.0f);
					SED_Checkbox("Denoise", aoSettings.mUseDenoiser);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Global Illumination"))
			{
				SED_Checkbox("DDGI Enabled", probeGridProperties.mDiffuseEnabled);
				SED_Checkbox("RTR Enabled", probeGridProperties.mSpecularEnabled);
				SED_FloatSlider("Energy Conservation", probeGridProperties.mEnergyConservation, 0.0f, 1.0f);
				SED_UintSlider("Num Rays Per Probe", probeGridProperties.mRaysPerProbe, 1, 1024);
				SED_FloatSlider("History Factor", probeGridProperties.mHistoryFactor, 0.0f, 1.0f);
				SED_FloatSlider("Normal Bias", probeGridProperties.mNormalBias, 0.0001f, 0.5f);
				SED_FloatSlider("Depth Sharpness", probeGridProperties.mDepthSharpness, 0.0f, 100.f);
				SED_FloatSlider("Global Roughness Multiplier", probeGridProperties.mGlobalRoughnessMultiplier, 0.0f, 1.0f);
				SED_Checkbox("Infinite Bounces", probeGridProperties.mInfiniteBounces);
				SED_Checkbox("Visualize Probes", probeGridProperties.mVisualizeProbes);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("TEMP Sun"))
			{
				SED_FloatField("Sun Direction", mActiveWorld->GetGraphicsWorld()->GetEnvironment()->GetSunDirection(), 0.1f, 1.0f, 0.01f); // temp
				SED_ColorPickerRGB("Sun Color", mActiveWorld->GetGraphicsWorld()->GetEnvironment()->GetSunColor());
				SED_FloatField("Sun Intensity", mActiveWorld->GetGraphicsWorld()->GetEnvironment()->GetSunIntensity());
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Utility"))
			{
				SED_Checkbox("Draw Grid", rendererSettings.mDrawGridHelper);
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	for (auto& panel : mWindows)
		panel->Draw();

	//mMaterialEditor->OnRender();
	ImGui::End();
	mImGui.Render(nullptr);

	return true;
}

void SED_Editor::Exit()
{

}
