#pragma once

struct ImDrawList;

enum class SED_GizmoMode
{
	Translate,
	Rotate,
	Scale
};

enum class SED_GizmoSpace
{
	World,
	Local
};

class SED_TransformationGizmo
{
public:
	SED_TransformationGizmo();
	~SED_TransformationGizmo();

	void BeginFrame(const SC_Vector4& aBounds);

	void SetViewAndProjection(const SC_Matrix& aView, const SC_Matrix& aProjection);
	void SetViewportPositionAndSize(const SC_Vector4& aPositionAndSize);

	bool Manipulate(SC_Matrix& aTransform, bool aShouldSnap = false, float aSnapValue = 0.0f);

	const SED_GizmoMode& GetManipulationMode() const		{ return mMode; }
	const SED_GizmoSpace& GetManipulationSpace() const		{ return mSpace; }
	void SetManipulationMode(const SED_GizmoMode& aMode)	{ mMode = aMode; }
	void SetManipulationSpace(const SED_GizmoSpace& aSpace) { mSpace = aSpace; }

private:

	void ComputeTripodAxisAndVisibility(const uint32 aAxisIndex, SC_Vector& aDirAxis, SC_Vector& aDirPlaneX, SC_Vector& aDirPlaneY, bool& aUnderAxisLimit, bool& aUnderPlaneLimit, const bool aLocalCoordinates = false);

	bool HandleTranslation(SC_Matrix& aTransform, bool aShouldSnap = false, float aSnapValue = 0.0f);
	bool HandleRotation(SC_Matrix& aTransform, bool aShouldSnap = false, float aSnapValue = 0.0f);
	bool HandleScale(SC_Matrix& aTransform, bool aShouldSnap = false, float aSnapValue = 0.0f);

	void DrawTranslationGizmo();
	void DrawRotationGizmo();
	void DrawScaleGizmo();

	SC_Vector2 WorldToScreen(const SC_Vector& aWorldPos) const;
	SC_Vector2 ModelToScreen(const SC_Vector& aWorldPos) const;

	SC_Matrix mWorldToCamera;
	SC_Matrix mCameraToWorld;
	SC_Matrix mCameraToClip;
	SC_Matrix mWorldToClip;
	SC_Matrix mModel;
	SC_Matrix mModelInv;
	SC_Matrix mModelLocal;
	SC_Matrix mMVP;

	SC_Vector4 mViewportPosAndSize;
	SC_Vector mCameraForward;

	SED_GizmoMode mMode;
	SED_GizmoSpace mSpace;

	float mScreenFactor;
	float mAspectRatio;

	ImDrawList* mDrawList;

	bool mIsActive;
	bool mIsEnabled;

};