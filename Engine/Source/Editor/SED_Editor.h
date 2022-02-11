#pragma once
#include "Application/SAF_Framework.h"
#include "RenderCore/ImGui/SR_ImGui.h"
#include "Graphics/View/SGfx_Camera.h"
#include "SED_Panel.h"
#include "SED_TransformationGizmo.h"

class SGF_World;
class SGF_Entity;
class SGfx_View;
class SED_ViewportPanel;
class SED_WorldHierarchyPanel;
class SED_PropertiesPanel;
class SED_MaterialEditor;

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
	SC_Array<SC_Ref<SED_Panel>> mPanels;

	SC_Ref<SED_ViewportPanel> mViewport;
	SC_Ref<SED_WorldHierarchyPanel> mWorldHierarchy;
	SC_Ref<SED_PropertiesPanel> mPropertiesPanel;
	
	SC_UniquePtr<SED_MaterialEditor> mMaterialEditor;

	// Current world
	SC_Ref<SGF_World> mActiveWorld;

	SGF_Entity* mSelectedEntity;
	SR_ImGui mImGui;

	SED_TransformationGizmo mGizmo;

	bool mIsDemoWindowOpen;
	bool mIsMetricsWindowOpen;
};