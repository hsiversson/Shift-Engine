#pragma once
#include "Graphics/View/SGfx_Camera.h"

class SED_ViewportPanel;
class SED_Camera : public SGfx_Camera
{
public:
	SED_Camera(SED_ViewportPanel* aParentViewport);

	void Update();

	void SetMovementSpeed(float aValue);
	float GetMovementSpeed() const;

	void SetBoostMultiplier(float aValue);
	float GetBoostMultiplier() const;

	void SetRotationSpeed(float aValue);
	float GetRotationSpeed() const;

private:
	enum ControlType
	{
		None,
		ArcBall,
		FreeLook,
		PlaneMove
	};

private:

	void UpdateArcBall();
	void UpdatePlaneMove();
	void UpdateFreeLook();

	ControlType mActiveControlType;
	SED_ViewportPanel* mParentViewport;
	SC_Vector2 mStoredMousePos;

	float mMovementSpeed;
	float mBoostMultiplier;
	float mRotationSpeed;
};

