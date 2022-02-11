#pragma once
#include "SED_Panel.h"
#include "Graphics/View/SGfx_Camera.h"

class SR_Texture;
class SGfx_View;
class SGfx_World;

class SED_TransformationGizmo;
class SED_ViewportPanel;
class SED_ViewportToolbar
{
public:
	SED_ViewportToolbar(const SED_ViewportPanel& aViewportPanel, SED_TransformationGizmo* aGizmo);

	void OnRender();
	
private:

	void DrawGizmoOptions();

	const SED_ViewportPanel& mViewportParent;
	SED_TransformationGizmo* mGizmo;

};

class SED_ViewportPanel : public SED_Panel 
{
public:
	SED_ViewportPanel(SGfx_World* aGfxWorld, SED_TransformationGizmo* aGizmo, const char* aId = "Viewport");
	~SED_ViewportPanel();

	const SC_Vector4& GetViewportBounds() const;
	const SC_Vector2& GetViewportSize() const;

	const SGfx_Camera& GetCamera() const;

	void Update() override;
	void OnRender() override;

	void SetCamera(SGfx_Camera* aCamera);
	SGfx_Camera* GetEditorCamera();

private:
	SED_ViewportToolbar mToolbar;

	SC_Vector4 mViewportBounds;
	SC_Vector2 mViewportSize;
	SGfx_Camera mEditorCamera;

	SGfx_World* mGfxWorld;
	SC_Ref<SGfx_View> mView;
	SGfx_Camera* mActiveCamera;

	SC_Ref<SR_Texture> mTexture;
	const char* mId;

	bool mIsFocused;
};