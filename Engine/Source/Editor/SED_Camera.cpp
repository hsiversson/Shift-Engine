#include "SED_Camera.h"
#include "InputOutput/KeyCodes/SC_KeyCodes.h"

SED_Camera::SED_Camera(SED_ViewportPanel* aParentViewport)
	: mActiveControlType(ControlType::None)
	, mParentViewport(aParentViewport)
	, mMovementSpeed(4.0f)
	, mBoostMultiplier(4.0f)
	, mRotationSpeed(10.0f)
{

}

void SED_Camera::Update()
{
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		mActiveControlType = ControlType::ArcBall;
	else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		mStoredMousePos = ImGui::GetMousePos();
		mActiveControlType = ControlType::FreeLook;
	}
	else if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
		mActiveControlType = ControlType::PlaneMove;

	switch (mActiveControlType)
	{
	case ControlType::ArcBall:
		UpdateArcBall();
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			mActiveControlType = ControlType::None;
		}
		break;
	case ControlType::FreeLook:
		UpdateFreeLook();
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			mActiveControlType = ControlType::None;
		}
		break;
	case ControlType::PlaneMove:
		UpdatePlaneMove();
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle))
		{
			mActiveControlType = ControlType::None;
		}
		break;
	}
}

void SED_Camera::SetMovementSpeed(float aValue)
{
	mMovementSpeed = aValue;
}

float SED_Camera::GetMovementSpeed() const
{
	return mMovementSpeed;
}

void SED_Camera::SetBoostMultiplier(float aValue)
{
	mBoostMultiplier = aValue;
}

float SED_Camera::GetBoostMultiplier() const
{
	return mBoostMultiplier;
}

void SED_Camera::SetRotationSpeed(float aValue)
{
	mRotationSpeed = aValue;
}

float SED_Camera::GetRotationSpeed() const
{
	return mRotationSpeed;
}

void SED_Camera::UpdateArcBall()
{

}

void SED_Camera::UpdatePlaneMove()
{

}

void SED_Camera::UpdateFreeLook()
{
	SC_Vector2 mousePos = ImGui::GetMousePos();
	SC_Vector2 mouseMoveDelta = mousePos - mStoredMousePos;
	mStoredMousePos = mousePos;

	float movementDelta = mMovementSpeed * SC_Time::gDeltaTime;
	float rotationDelta = mRotationSpeed * SC_Time::gDeltaTime;

	if (ImGui::IsKeyDown(static_cast<uint32>(SC_KeyCode::Shift)))
	{
		movementDelta *= mBoostMultiplier;
	}

	// FORWARD / BACKWARD
	if (ImGui::IsKeyDown(static_cast<uint32>(SC_KeyCode::W)))
	{
		Move(GetForward(), movementDelta);
	}
	if (ImGui::IsKeyDown(static_cast<uint32>(SC_KeyCode::S)))
	{
		Move(GetForward(), -movementDelta);
	}

	// LEFT / RIGHT
	if (ImGui::IsKeyDown(static_cast<uint32>(SC_KeyCode::A)))
	{
		Move(GetRight(), -movementDelta);
	}
	if (ImGui::IsKeyDown(static_cast<uint32>(SC_KeyCode::D)))
	{
		Move(GetRight(), movementDelta);
	}

	// UP / DOWN
	if (ImGui::IsKeyDown(static_cast<uint32>(SC_KeyCode::Q)))
	{
		Move(SC_Vector::UpVector(), -movementDelta);
	}
	if (ImGui::IsKeyDown(static_cast<uint32>(SC_KeyCode::E)))
	{
		Move(SC_Vector::UpVector(), movementDelta);
	}

	float pitch = -mouseMoveDelta.y * rotationDelta;
	float yaw = -mouseMoveDelta.x * rotationDelta;
	Rotate({ pitch, yaw, 0.0f });
}
