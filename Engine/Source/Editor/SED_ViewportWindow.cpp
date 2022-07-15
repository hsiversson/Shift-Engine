#include "SED_ViewportWindow.h"
#include "Graphics/World/SGfx_World.h"
#include "Graphics/View/SGfx_View.h"
#include "Graphics/Renderer/SGfx_Renderer.h"
#include "SED_TransformationGizmo.h"

#include "ImGuizmo/ImGuizmo.h"

SED_ViewportToolbar::SED_ViewportToolbar(SED_ViewportWindow& aViewportPanel, SED_TransformationGizmo& aGizmo)
	: mViewportParent(aViewportPanel)
	, mGizmo(aGizmo)
{

}

void SED_ViewportToolbar::OnDraw()
{
	DrawGizmoOptions();
}

void SED_ViewportToolbar::DrawGizmoOptions()
{
	float fontHeight = SED_GetCurrentFontSize();
	ImVec2 buttonTextSize = ImGui::CalcTextSize("World", nullptr, true);
	ImVec2 buttonSize = { buttonTextSize.x + 10.f,  fontHeight + 10.f };

	ImVec2 spaceButtonSize;
	const ImVec4 selectedButtonColor = ImVec4(32 / 255.f, 128 / 255.f, 32 / 255.f, 0.75f);
	const ImVec4 defaultButtonColor = ImVec4(51 / 255.f, 51 / 255.f, 55 / 255.f, 0.75f);

	bool isLocal = mGizmo.GetManipulationSpace() == SED_GizmoSpace::Local;

	const SC_Vector4& viewportBounds = mViewportParent.GetViewportBounds();
	ImVec2 gizmoSpaceButtonPos = { viewportBounds.z - buttonSize.x - 200.f, viewportBounds.y + 10.f };
	ImGui::SetCursorScreenPos(gizmoSpaceButtonPos);

	// World/Local space selector
	if (!isLocal)
		ImGui::PushStyleColor(ImGuiCol_Button, selectedButtonColor);

	if (SED_Button("World", spaceButtonSize))
		mGizmo.SetManipulationSpace(SED_GizmoSpace::World);

	if (!isLocal)
		ImGui::PopStyleColor();

	SED_SameLine(0.0f, 2.0f);

	if (isLocal)
		ImGui::PushStyleColor(ImGuiCol_Button, selectedButtonColor);

	if (SED_Button("Local", spaceButtonSize))
		mGizmo.SetManipulationSpace(SED_GizmoSpace::Local);

	if (isLocal)
		ImGui::PopStyleColor();

	SED_SameLine(0.0f, 4.0f);

	if (SED_BeginComboBox("Camera", "Camera"))
	{
		float speed = mViewportParent.mEditorCamera.GetMovementSpeed();
		if (SED_FloatField("Speed", speed, 1.0f, 1.0f, 128.0f))
			mViewportParent.mEditorCamera.SetMovementSpeed(speed);

		float boost = mViewportParent.mEditorCamera.GetBoostMultiplier();
		if (SED_FloatField("Boost", boost, 1.0f, 1.0f, 1024.0f))
			mViewportParent.mEditorCamera.SetBoostMultiplier(boost);

		SED_EndComboBox();
	}

	SED_SameLine(0.0f, 4.0f);

	if (SED_BeginComboBox("Mode", "Translate"))
	{
		SED_Selectable("Translate");
		SED_Selectable("Rotate");
		SED_Selectable("Scale");

		SED_EndComboBox();
	}
}

SED_ViewportWindow::SED_ViewportWindow(SGfx_World* aGfxWorld, const char* aId)
	: mToolbar(*this, mGizmo)
	, mEditorCamera(this)
	, mGfxWorld(aGfxWorld)
	, mId(aId)
	, mCameraSpeed(2.0f)
	, mBoostSpeed(4.0f)
{
	mView = mGfxWorld->CreateView();
	mView->SetMainView(true);

	mEditorCamera.SetPerspectiveProjection({ 1920.f, 1080.f }, 0.01f, 10000.f, 90.0f);
	mEditorCamera.SetPosition({ -10.0f, 2.0f, 0.0f });
	mEditorCamera.LookAt({ 0.f, 1.f, 0.f });
	mActiveCamera = &mEditorCamera;
}

SED_ViewportWindow::~SED_ViewportWindow()
{
	mGfxWorld->DestroyView(mView);
}

const SC_Vector4& SED_ViewportWindow::GetViewportBounds() const
{
	return mViewportBounds;
}

const SC_Vector2& SED_ViewportWindow::GetViewportSize() const
{
	return mViewportSize;
}

const SGfx_Camera& SED_ViewportWindow::GetCamera() const
{
	return *mActiveCamera;
}

void SED_ViewportWindow::SetCamera(SGfx_Camera* aCamera)
{
	mActiveCamera = aCamera;
}

SGfx_Camera* SED_ViewportWindow::GetEditorCamera()
{
	return &mEditorCamera;
}

void SED_ViewportWindow::SetSelectedEntity(const SGF_Entity& aEntity)
{
	mGizmo.SetSelectedEntity(aEntity);
}

const char* SED_ViewportWindow::GetWindowName() const
{
	return mId;
}

void SED_ViewportWindow::OnUpdate()
{
	if (mIsFocused && mActiveCamera == &mEditorCamera)
	{
		mEditorCamera.Update();
	}

	if (mActiveCamera)
	{
		// if viewport bounds changed, update camera and render-viewport to account for it
		if (mViewportSize != mActiveCamera->GetSize() && mViewportSize > 0.0f)
		{
			mActiveCamera->SetPerspectiveProjection(mViewportSize, 0.01f, 10000.f);
			mActiveCamera->SetViewportOffset({ (int32)mViewportBounds.x, (int32)mViewportBounds.y });
		}

		mView->SetCamera(*mActiveCamera);
	}
	mGfxWorld->PrepareView(mView);
	mGfxWorld->RenderView(mView);
}

void SED_ViewportWindow::OnDraw()
{
	SC_Vector2 viewportMinRegion = GetContentRegionMin();
	SC_Vector2 viewportMaxRegion = GetContentRegionMax();
	SC_Vector2 viewportOffset = GetPosition();
	mViewportBounds.x = viewportMinRegion.x + viewportOffset.x;
	mViewportBounds.y = viewportMinRegion.y + viewportOffset.y;
	mViewportBounds.z = viewportMaxRegion.x + viewportOffset.x;
	mViewportBounds.w = viewportMaxRegion.y + viewportOffset.y;

	SC_Vector2 viewportPanelSize = GetAvailableContentRegion();
	mViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

	const SC_IntVector& textureSize = mGfxWorld->GetRenderer()->GetScreenColor()->GetResource()->GetProperties().mSize;

	SED_Image(mGfxWorld->GetRenderer()->GetScreenColor(), viewportPanelSize, ImVec2(0, 0), ImVec2(viewportPanelSize.x / (float)textureSize.x, viewportPanelSize.y / (float)textureSize.y));

	mToolbar.OnDraw();
	mIsFocused = IsFocused();

	const SGfx_ViewConstants constants = GetCamera().GetViewConstants();
	const SC_Vector4 viewportBounds = GetViewportBounds();
	mGizmo.SetViewportPositionAndSize(SC_Vector4(viewportBounds.x, viewportBounds.y, viewportBounds.z - viewportBounds.x, viewportBounds.w - viewportBounds.y));
	mGizmo.SetViewAndProjection(constants.mWorldToCamera, constants.mCameraToClip);

	mGizmo.Manipulate();
}

void SED_ViewportWindow::RecieveMessage(const SC_Message& aMsg)
{
	if (mIsFocused && aMsg.mType == SC_MessageType::Scroll)
	{
		mBoostSpeed += aMsg.Get<float>() * 10.f;
		mBoostSpeed = SC_Clamp(mBoostSpeed, 4.0f, 32000.f);
	}
}
