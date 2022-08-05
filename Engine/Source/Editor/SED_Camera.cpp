#include "SED_Camera.h"
#include "InputOutput/KeyCodes/SC_KeyCodes.h"

#include "SED_ViewportWindow.h"

SED_Camera::SED_Camera(SED_ViewportWindow* aParentViewport)
	: mActiveControlType(ControlType::None)
	, mParentViewport(aParentViewport)
	, mDistanceToPivotPosition(1.0f)
	, mMovementSpeed(4.0f)
	, mBoostMultiplier(4.0f)
	, mRotationSpeed(120.0f)
{

}

void SED_Camera::Update()
{
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		mActiveControlType = ControlType::ArcBall;
	else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		mActiveControlType = ControlType::FreeLook;
	else if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
		mActiveControlType = ControlType::PlaneMove;

	switch (mActiveControlType)
	{
	case ControlType::ArcBall:
		UpdateArcBall();
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			mActiveControlType = ControlType::None;
			mTargetPosition = GetPosition();
		}
		break;
	case ControlType::FreeLook:
		UpdateFreeLook();
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			mActiveControlType = ControlType::None;
			SetPivot(GetPosition() + GetForward() * mDistanceToPivotPosition);
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

	if (mActiveControlType != ControlType::ArcBall)
		MoveTowards(mTargetPosition, SC_Time::gDeltaTime, 16.0f);

	if (ImGui::IsKeyDown((int32)SC_KeyCode::Control))
	{
		ImGuiIO& io = ImGui::GetIO();
		float newFov = GetFov();
		if (io.MouseWheel != 0.0f)
			newFov = SC_Min(SC_Max(newFov - io.MouseWheel, 30.0f), 120.f);

		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Middle))
			newFov = 75.0f;

		SetPerspectiveProjection(GetSize(), GetNear(), GetFar(), newFov);
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

void SED_Camera::SetPivot(const SC_Vector& aPivot)
{
	mArcBallPivotPosition = aPivot;
	mDistanceToPivotPosition = (mArcBallPivotPosition - GetPosition()).Length();
}

const SC_Vector& SED_Camera::GetPivot() const
{
	return mArcBallPivotPosition;
}

void SED_Camera::UpdateArcBall()
{
	ImGuiIO& io = ImGui::GetIO();
	SC_Vector2 mouseMoveDelta = io.MouseDelta;

	SC_Vector pos = GetPosition();
	SC_Vector pivot = GetPivot();

	SC_Vector2 deltaAngles = SC_Vector2(2.0f * SC_Math::PI) / mParentViewport->GetViewportSize();
	SC_Vector2 angles = mouseMoveDelta * deltaAngles;

	float cosAngle = GetForward().Dot(SC_Vector::UpVector());
	if (cosAngle * SC_Math::Sign(deltaAngles.y) > 0.99f)
		deltaAngles.y = 0.0f;

	SC_Matrix rotateX = SC_Matrix::CreateRotation(SC_Vector::UpVector(), angles.x);
	SC_Matrix rotateY = SC_Matrix::CreateRotation(GetRight(), angles.y);

	pos = ((pos - pivot) * rotateX) + pivot;
	SC_Vector finalPos = ((pos - pivot) * rotateY) + pivot;

	SetPosition(finalPos);
	LookAt(pivot);
}

void SED_Camera::UpdatePlaneMove()
{

}

void SED_Camera::UpdateFreeLook()
{
	float speed = mMovementSpeed * SC_Time::gDeltaTime;
	if (ImGui::IsKeyDown(static_cast<uint32>(SC_KeyCode::Shift)))
	{
		speed *= mBoostMultiplier;
	}

	// FORWARD / BACKWARD
	if (ImGui::IsKeyDown(static_cast<uint32>(SC_KeyCode::W)))
	{
		mTargetPosition += GetForward() * speed;

	}
	if (ImGui::IsKeyDown(static_cast<uint32>(SC_KeyCode::S)))
	{
		mTargetPosition -= GetForward() * speed;
	}

	// LEFT / RIGHT
	if (ImGui::IsKeyDown(static_cast<uint32>(SC_KeyCode::A)))
	{
		mTargetPosition -= GetRight() * speed;
	}
	if (ImGui::IsKeyDown(static_cast<uint32>(SC_KeyCode::D)))
	{
		mTargetPosition += GetRight() * speed;
	}

	// UP / DOWN
	if (ImGui::IsKeyDown(static_cast<uint32>(SC_KeyCode::Q)))
	{
		mTargetPosition -= SC_Vector::UpVector() * speed;
	}
	if (ImGui::IsKeyDown(static_cast<uint32>(SC_KeyCode::E)))
	{
		mTargetPosition += SC_Vector::UpVector() * speed;
	}

	ImGuiIO& io = ImGui::GetIO();
	SC_Vector2 deltaScale = SC_Vector2(1.0f) / mParentViewport->GetViewportSize();
	SC_Vector2 mouseMoveDelta = SC_Vector2(io.MouseDelta) * deltaScale;

	if (mouseMoveDelta.x != 0.0f || mouseMoveDelta.y != 0.0f)
	{
		float rotationDelta = mRotationSpeed;
		float pitch = -mouseMoveDelta.y * rotationDelta;
		float yaw = -mouseMoveDelta.x * rotationDelta;
		Rotate({ pitch, yaw, 0.0f });
	}
}
