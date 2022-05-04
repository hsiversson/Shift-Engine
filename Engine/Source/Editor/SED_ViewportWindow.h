#pragma once
#include "SED_Window.h"
#include "Common/MessageQueue/SC_MessageListener.h"
#include "SED_Camera.h"

class SR_Texture;
class SGfx_View;
class SGfx_World;

class SED_TransformationGizmo;
class SED_ViewportWindow;
class SED_ViewportToolbar
{
public:
	SED_ViewportToolbar(SED_ViewportWindow& aViewportPanel, SED_TransformationGizmo* aGizmo);

	void OnDraw();
	
private:

	void DrawGizmoOptions();

	SED_ViewportWindow& mViewportParent;
	SED_TransformationGizmo* mGizmo;

};

class SED_ViewportWindow : public SED_Window, SC_MessageListener
{
	friend class SED_ViewportToolbar;
public:
	SED_ViewportWindow(SGfx_World* aGfxWorld, SED_TransformationGizmo* aGizmo, const char* aId = "Viewport");
	~SED_ViewportWindow();

	const SC_Vector4& GetViewportBounds() const;
	const SC_Vector2& GetViewportSize() const;

	const SGfx_Camera& GetCamera() const;


	void SetCamera(SGfx_Camera* aCamera);
	SGfx_Camera* GetEditorCamera();

	const char* GetWindowName() const override;

protected:
	void OnUpdate() override;
	void OnDraw() override;

protected:
	void RecieveMessage(const SC_Message& aMsg) override;

private:
	SED_ViewportToolbar mToolbar;

	SC_Vector4 mViewportBounds;
	SC_Vector2 mViewportSize;
	SED_Camera mEditorCamera;

	SGfx_World* mGfxWorld;
	SC_Ref<SGfx_View> mView;
	SGfx_Camera* mActiveCamera;

	SC_Ref<SR_Texture> mTexture;
	const char* mId;

	float mCameraSpeed;
	float mBoostSpeed;
	bool mIsFocused;
};