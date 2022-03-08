#include "SED_ViewportPanel.h"
#include "Graphics/World/SGfx_World.h"
#include "Graphics/View/SGfx_View.h"
#include "Graphics/Renderer/SGfx_Renderer.h"
#include "SED_TransformationGizmo.h"

SED_ViewportToolbar::SED_ViewportToolbar(SED_ViewportPanel& aViewportPanel, SED_TransformationGizmo* aGizmo)
	: mViewportParent(aViewportPanel)
	, mGizmo(aGizmo)
{

}

void SED_ViewportToolbar::OnRender()
{
	DrawGizmoOptions();
}

void SED_ViewportToolbar::DrawGizmoOptions()
{
	if (!mGizmo)
		return;

	float fontHeight = ImGui::GetFontSize();
	ImVec2 buttonTextSize = ImGui::CalcTextSize("World", nullptr, true);
	ImVec2 buttonSize = { buttonTextSize.x + 10.f,  fontHeight + 10.f };

	ImVec2 spaceButtonSize;
	const ImVec4 selectedButtonColor = ImVec4(32 / 255.f, 128 / 255.f, 32 / 255.f, 0.75f);
	const ImVec4 defaultButtonColor = ImVec4(51 / 255.f, 51 / 255.f, 55 / 255.f, 0.75f);

	bool isLocal = mGizmo->GetManipulationSpace() == SED_GizmoSpace::Local;

	const SC_Vector4& viewportBounds = mViewportParent.GetViewportBounds();
	ImVec2 gizmoSpaceButtonPos = { viewportBounds.z - buttonSize.x - 200.f, viewportBounds.y + 10.f };
	ImGui::SetCursorScreenPos(gizmoSpaceButtonPos);

	// World/Local space selector
	if (!isLocal)
		ImGui::PushStyleColor(ImGuiCol_Button, selectedButtonColor);

	if (ImGui::Button("World", spaceButtonSize))
		mGizmo->SetManipulationSpace(SED_GizmoSpace::World);

	if (!isLocal)
		ImGui::PopStyleColor();

	ImGui::SameLine(0.0f, 2.0f);

	if (isLocal)
		ImGui::PushStyleColor(ImGuiCol_Button, selectedButtonColor);

	if (ImGui::Button("Local", spaceButtonSize))
		mGizmo->SetManipulationSpace(SED_GizmoSpace::Local);

	if (isLocal)
		ImGui::PopStyleColor();

	ImGui::SameLine(0.0f, 4.0f);

	if (ImGui::BeginCombo("Camera", "Camera"))
	{
		float speed = mViewportParent.mEditorCamera.GetMovementSpeed();
		if (ImGui::DragFloat("Speed", &speed, 1.0f, 1.0f, 128.0f))
			mViewportParent.mEditorCamera.SetMovementSpeed(speed);

		float boost = mViewportParent.mEditorCamera.GetBoostMultiplier();
		if (ImGui::DragFloat("Boost", &boost, 1.0f, 1.0f, 1024.0f))
			mViewportParent.mEditorCamera.SetBoostMultiplier(boost);

		ImGui::EndCombo();
	}

	ImGui::SameLine(0.0f, 4.0f);

	if (ImGui::BeginCombo("Mode", "Translate"))
	{
		ImGui::Selectable("Translate");
		ImGui::Selectable("Rotate");
		ImGui::Selectable("Scale");

		ImGui::EndCombo();
	}
}

SED_ViewportPanel::SED_ViewportPanel(SGfx_World* aGfxWorld, SED_TransformationGizmo* aGizmo, const char* aId)
	: mToolbar(*this, aGizmo)
	, mEditorCamera(this)
	, mGfxWorld(aGfxWorld)
	, mId(aId)
	, mCameraSpeed(2.0f)
	, mBoostSpeed(4.0f)
{
	mView = mGfxWorld->CreateView();
	mView->SetMainView(true);

	mEditorCamera.SetPerspectiveProjection({ 1920.f, 1080.f }, 0.01f, 1000.f, 90.0f);
	mEditorCamera.SetPosition({ 10.0f, 2.0f, 0.0f });
	mEditorCamera.LookAt({ 0.f, 1.f, 0.f });
	mActiveCamera = &mEditorCamera;
}

SED_ViewportPanel::~SED_ViewportPanel()
{
	mGfxWorld->DestroyView(mView);
}

const SC_Vector4& SED_ViewportPanel::GetViewportBounds() const
{
	return mViewportBounds;
}

const SC_Vector2& SED_ViewportPanel::GetViewportSize() const
{
	return mViewportSize;
}

const SGfx_Camera& SED_ViewportPanel::GetCamera() const
{
	return *mActiveCamera;
}

void SED_ViewportPanel::Update()
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
			mActiveCamera->SetPerspectiveProjection(mViewportSize, 0.01f, 1000.f);
			mActiveCamera->SetViewportOffset({ (int32)mViewportBounds.x, (int32)mViewportBounds.y });
		}

		mView->SetCamera(*mActiveCamera);
	}
	mGfxWorld->PrepareView(mView.get());
	mGfxWorld->RenderView(mView.get());
}

void SED_ViewportPanel::OnRender()
{
	ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiCond_FirstUseEver);
	ImGui::Begin(mId);

	ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
	ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	ImVec2 viewportOffset = ImGui::GetWindowPos();
	mViewportBounds.x = viewportMinRegion.x + viewportOffset.x;
	mViewportBounds.y = viewportMinRegion.y + viewportOffset.y;
	mViewportBounds.z = viewportMaxRegion.x + viewportOffset.x;
	mViewportBounds.w = viewportMaxRegion.y + viewportOffset.y;

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	mViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

	const SC_IntVector& textureSize = mGfxWorld->GetRenderer()->GetScreenColor()->GetResource()->GetProperties().mSize;

	ImGui::Image(mGfxWorld->GetRenderer()->GetScreenColor().get(), viewportPanelSize, ImVec2(0,0), ImVec2(viewportPanelSize.x / (float)textureSize.x, viewportPanelSize.y / (float)textureSize.y));

	mToolbar.OnRender();
	mIsFocused = ImGui::IsWindowFocused();
	ImGui::End();
}

void SED_ViewportPanel::SetCamera(SGfx_Camera* aCamera)
{
	mActiveCamera = aCamera;
}

SGfx_Camera* SED_ViewportPanel::GetEditorCamera()
{
	return &mEditorCamera;
}

void SED_ViewportPanel::RecieveMessage(const SC_Message& aMsg)
{
	if (mIsFocused && aMsg.mType == SC_MessageType::Scroll)
	{
		mBoostSpeed += aMsg.Get<float>() * 10.f;
		mBoostSpeed = SC_Clamp(mBoostSpeed, 4.0f, 32000.f);
	}
}
