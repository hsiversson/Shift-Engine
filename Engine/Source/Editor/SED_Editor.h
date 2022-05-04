#pragma once
#include "Application/SAF_Framework.h"
#include "RenderCore/ImGui/SR_ImGui.h"
#include "Graphics/View/SGfx_Camera.h"
#include "SED_Window.h"
#include "SED_TransformationGizmo.h"
#include "GameFramework/Entity/SGF_Entity.h"

class SGF_World;
class SGfx_View;
class SED_ViewportWindow;
class SED_WorldHierarchyWindow;
class SED_PropertiesWindow;
class SED_MaterialEditor;
class SED_MetricsWindow;

class SED_Editor : public SAF_AppCallbacks
{
public:
	SED_Editor();
	~SED_Editor();

	virtual bool Init() override;
	virtual bool Update() override;
	virtual bool Render() override;
	virtual void Exit() override;

private:
	// Panels
	SC_Array<SC_Ref<SED_Window>> mWindows;

	SC_Ref<SED_ViewportWindow> mViewport;
	SC_Ref<SED_WorldHierarchyWindow> mWorldHierarchy;
	SC_Ref<SED_PropertiesWindow> mPropertiesPanel;
	SC_Ref<SED_MetricsWindow> mMetricsWindow;
	
	SC_UniquePtr<SED_MaterialEditor> mMaterialEditor;

	// Current world
	SC_Ref<SGF_World> mActiveWorld;

	SGF_Entity mSelectedEntity;
	SR_ImGui mImGui;

	SED_TransformationGizmo mGizmo;

	bool mIsDemoWindowOpen;
};