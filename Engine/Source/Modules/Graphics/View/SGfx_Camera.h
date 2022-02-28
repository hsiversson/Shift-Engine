#pragma once

struct alignas(16) SGfx_ViewConstants
{
	SC_Matrix mWorldToClip;
	SC_Matrix mWorldToClip_NoJitter;
	SC_Matrix mClipToWorld;

	SC_Matrix mWorldToCamera;
	SC_Matrix mCameraToWorld;

	SC_Matrix mCameraToClip;
	SC_Matrix mClipToCamera;

	SC_Matrix mPrevWorldToClip;
	SC_Matrix mPrevWorldToClip_NoJitter;
	SC_Matrix mPrevClipToWorld;

	SC_Matrix mPrevWorldToCamera;
	SC_Matrix mPrevCameraToWorld;

	SC_Matrix mPrevCameraToClip;
	SC_Matrix mPrevClipToCamera;

	SC_Vector4 mPixelToClipScaleAndOffset;

	SC_Vector4 mViewportPosAndInvSize;
	SC_Vector4 mViewportSizeAndScale;

	SC_Vector4 mRenderTargetSizeAndInvSize;

	SC_Vector mCameraPosition;
	float mFov;
};

class SGfx_Frustum
{
	enum FrustumPlanes
	{
		FrustumPlane_Near = 0,
		FrustumPlane_Right,
		FrustumPlane_Left,
		FrustumPlane_Bottom,
		FrustumPlane_Top,
		FrustumPlane_Far,
		FrustumPlanes_COUNT,
	};

public:
	SGfx_Frustum();
	~SGfx_Frustum();

	void Update(const SC_Matrix& aViewProj);

	bool Intersects(const SC_AABB& aBoundingBox) const;
	bool Intersects(const SC_Sphere& aBoundingSphere) const;


private:
	SC_Plane mPlanes[FrustumPlanes_COUNT];
};

class SGfx_Camera
{
public:
	SGfx_Camera();
	~SGfx_Camera();

	void SetProjectionJitter(const SC_Vector2& aJitter) const;
	const SC_Vector2& GetProjectionJitter() const;

	void SetPerspectiveProjection(const SC_Vector2& aSize, float aNear, float aFar, float aFov = 75.f);
	void SetOrthogonalProjection(const SC_Vector2& aSize, float aNear, float aFar);

	void SetViewportOffset(const SC_IntVector2& aOffset);

	void Move(const SC_Vector& aDirection, float aLength);
	void Rotate(const SC_Vector& aRotationAngles);
	void Rotate(const SC_Vector& aAxis, float aValue);

	void SetPosition(const SC_Vector& aPos);
	SC_Vector GetPosition() const;

	void LookAt(const SC_Vector& aTargetPos, const SC_Vector& aUpVector = SC_Vector::UpVector());

	SGfx_ViewConstants GetViewConstants() const;
	const SGfx_Frustum& GetFrustum() const;

	const SC_Vector& GetForward() const;
	const SC_Vector& GetUp() const;
	const SC_Vector& GetRight() const;

	const SC_Vector2& GetSize() const;
	float GetNear() const;
	float GetFar() const;

	SC_Matrix GetProjectionNoInvZ() const;

	void GetCornersOnPlane(float aZ, SC_Vector* aOutCorners) const;
private:
	void Update() const;

	mutable SGfx_ViewConstants mCachedConstants;

	SC_Matrix mTransform;
	SC_Matrix mProjection;
	SC_Matrix mInvProjection;
	mutable SGfx_Frustum mFrustum;

	mutable SC_Vector mForward;
	mutable SC_Vector mUp;
	mutable SC_Vector mRight;

	mutable SC_Vector2 mProjectionJitter;
	SC_IntVector2 mViewportOffset;
	SC_Vector2 mSize;
	float mFov;
	float mNear;
	float mFar;
	float mAspectRatio;

	bool mIsOrthogonal : 1;
	mutable bool mIsDirty : 1;
};

