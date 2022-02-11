#include "SED_ViewportPanel.h"
#include "Graphics/World/SGfx_World.h"
#include "Graphics/View/SGfx_View.h"
#include "Graphics/Renderer/SGfx_Renderer.h"
#include "SED_TransformationGizmo.h"

SED_ViewportToolbar::SED_ViewportToolbar(const SED_ViewportPanel& aViewportPanel, SED_TransformationGizmo* aGizmo)
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
	ImVec2 gizmoSpaceButtonPos = { viewportBounds.z - buttonSize.x - 150.f, viewportBounds.y + 10.f };
	ImGui::SetCursorScreenPos(gizmoSpaceButtonPos);

	// World/Local space selector
	if (!isLocal)
		ImGui::PushStyleColor(ImGuiCol_Button, selectedButtonColor);

	if (ImGui::Button("World", spaceButtonSize))
		mGizmo->SetManipulationSpace(SED_GizmoSpace::World);

	if (!isLocal)
		ImGui::PopStyleColor();

	ImGui::SameLine(0.0f, 1.0f);

	if (isLocal)
		ImGui::PushStyleColor(ImGuiCol_Button, selectedButtonColor);

	if (ImGui::Button("Local", spaceButtonSize))
		mGizmo->SetManipulationSpace(SED_GizmoSpace::Local);

	if (isLocal)
		ImGui::PopStyleColor();

	ImGui::SameLine(0.0f, 4.0f);

	if (ImGui::BeginCombo("V", "Translate", ImGuiComboFlags_NoArrowButton))
	{
		ImGui::Selectable("Translate");
		ImGui::Selectable("Rotate");
		ImGui::Selectable("Scale");

		ImGui::EndCombo();
	}
}

SED_ViewportPanel::SED_ViewportPanel(SGfx_World* aGfxWorld, SED_TransformationGizmo* aGizmo, const char* aId)
	: mToolbar(*this, aGizmo)
	, mGfxWorld(aGfxWorld)
	, mId(aId)
{
	mView = mGfxWorld->CreateView();
	mView->SetMainView(true);

	mEditorCamera.SetPerspectiveProjection({ 1920.f, 1080.f }, 0.01f, 1000.f);
	mEditorCamera.SetPosition({ 0, 1.f, -2.f });
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
		float delta = 2.0f * SC_Time::gDeltaTime;

		// if mouse is inside viewport and mousekey is held down: activate controller
		// choose between: arcball, freelook and middle mouse dragging

		if (GetKeyState(VK_LSHIFT) & 0x8000)
		{
			delta *= 4.f;
		}

		// FORWARD / BACKWARD
		if (GetKeyState('W') & 0x8000)
		{
			mEditorCamera.Move(mEditorCamera.GetForward(), delta);
		}
		if (GetKeyState('S') & 0x8000)
		{
			mEditorCamera.Move(mEditorCamera.GetForward(), -delta);
		}

		// LEFT / RIGHT
		if (GetKeyState('A') & 0x8000)
		{
			mEditorCamera.Move(mEditorCamera.GetRight(), -delta);
		}
		if (GetKeyState('D') & 0x8000)
		{
			mEditorCamera.Move(mEditorCamera.GetRight(), delta);
		}

		// UP / DOWN
		if (GetKeyState(VK_LCONTROL) & 0x8000)
		{
			mEditorCamera.Move(mEditorCamera.GetUp(), -delta);
		}
		if (GetKeyState(VK_SPACE) & 0x8000)
		{
			mEditorCamera.Move(mEditorCamera.GetUp(), delta);
		}

		if (GetKeyState('Q') & 0x8000)
		{
			mEditorCamera.Rotate(SC_Vector::UpVector(), delta * 0.25f);
		}
		if (GetKeyState('E') & 0x8000)
		{
			mEditorCamera.Rotate(SC_Vector::UpVector(), -delta * 0.25f);
		}
		if (GetKeyState('Z') & 0x8000)
		{
			mEditorCamera.Rotate(mEditorCamera.GetRight(), delta * 0.25f);
		}
		if (GetKeyState('X') & 0x8000)
		{
			mEditorCamera.Rotate(mEditorCamera.GetRight(), -delta * 0.25f);
		}
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
