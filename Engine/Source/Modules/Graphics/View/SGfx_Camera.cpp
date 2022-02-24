#include "SGfx_Camera.h"


SGfx_Frustum::SGfx_Frustum()
{
}

SGfx_Frustum::~SGfx_Frustum()
{
}

void SGfx_Frustum::Update(const SC_Matrix& aViewProj)
{
	mPlanes[FrustumPlane_Right].mNormal.x = aViewProj[3] - aViewProj[0];
	mPlanes[FrustumPlane_Right].mNormal.y = aViewProj[7] - aViewProj[4];
	mPlanes[FrustumPlane_Right].mNormal.z = aViewProj[11] - aViewProj[8];
	mPlanes[FrustumPlane_Right].mDistance = aViewProj[15] - aViewProj[12];
	mPlanes[FrustumPlane_Right].Normalize();

	mPlanes[FrustumPlane_Left].mNormal.x = aViewProj[3] + aViewProj[0];
	mPlanes[FrustumPlane_Left].mNormal.y = aViewProj[7] + aViewProj[4];
	mPlanes[FrustumPlane_Left].mNormal.z = aViewProj[11] + aViewProj[8];
	mPlanes[FrustumPlane_Left].mDistance = aViewProj[15] + aViewProj[12];
	mPlanes[FrustumPlane_Left].Normalize();

	mPlanes[FrustumPlane_Top].mNormal.x = aViewProj[3] - aViewProj[1];
	mPlanes[FrustumPlane_Top].mNormal.y = aViewProj[7] - aViewProj[5];
	mPlanes[FrustumPlane_Top].mNormal.z = aViewProj[11] - aViewProj[9];
	mPlanes[FrustumPlane_Top].mDistance = aViewProj[15] - aViewProj[13];
	mPlanes[FrustumPlane_Top].Normalize();

	mPlanes[FrustumPlane_Bottom].mNormal.x = aViewProj[3] + aViewProj[1];
	mPlanes[FrustumPlane_Bottom].mNormal.y = aViewProj[7] + aViewProj[5];
	mPlanes[FrustumPlane_Bottom].mNormal.z = aViewProj[11] + aViewProj[9];
	mPlanes[FrustumPlane_Bottom].mDistance = aViewProj[15] + aViewProj[13];
	mPlanes[FrustumPlane_Bottom].Normalize();

	mPlanes[FrustumPlane_Near].mNormal.x = aViewProj[3] - aViewProj[2];
	mPlanes[FrustumPlane_Near].mNormal.y = aViewProj[7] - aViewProj[6];
	mPlanes[FrustumPlane_Near].mNormal.z = aViewProj[11] - aViewProj[10];
	mPlanes[FrustumPlane_Near].mDistance = aViewProj[15] - aViewProj[14];
	mPlanes[FrustumPlane_Near].Normalize();

	mPlanes[FrustumPlane_Far].mNormal.x = aViewProj[2];
	mPlanes[FrustumPlane_Far].mNormal.y = aViewProj[6];
	mPlanes[FrustumPlane_Far].mNormal.z = aViewProj[10];
	mPlanes[FrustumPlane_Far].mDistance = aViewProj[14];
	mPlanes[FrustumPlane_Far].Normalize();
}

bool SGfx_Frustum::Intersects(const SC_AABB& aBoundingBox) const
{
	bool aResult = true;
	for (uint32 planeIdx = 0; planeIdx < FrustumPlanes_COUNT; ++planeIdx)
	{
		SC_Vector AxisVert;
		if (mPlanes[planeIdx].mNormal.x < 0.0f)
			AxisVert.x = aBoundingBox.mMin.x;
		else
			AxisVert.x = aBoundingBox.mMax.x;

		if (mPlanes[planeIdx].mNormal.y < 0.0f)
			AxisVert.y = aBoundingBox.mMin.y;
		else
			AxisVert.y = aBoundingBox.mMax.y;

		if (mPlanes[planeIdx].mNormal.z < 0.0f)
			AxisVert.z = aBoundingBox.mMin.z;
		else
			AxisVert.z = aBoundingBox.mMax.z;

		if (mPlanes[planeIdx].DistanceToPlane(AxisVert) < 0.0f)
		{
			aResult = false;
			break;
		}
	}

	return aResult;
}

bool SGfx_Frustum::Intersects(const SC_Sphere& aBoundingSphere) const
{
	float minDist = 0.0f;
	for (uint32 planeIdx = 0; planeIdx < FrustumPlanes_COUNT; ++planeIdx)
	{
		float dist = mPlanes[planeIdx].DistanceToPlane(aBoundingSphere.mCenter);
		minDist = SC_Min(minDist, dist);
	}

	return minDist > (-aBoundingSphere.mRadius);
}

SGfx_Camera::SGfx_Camera()
	: mProjectionJitter(0.0f)
	, mViewportOffset(0)
	, mFov(0.f)
	, mNear(0.f)
	, mFar(0.f)
	, mIsOrthogonal(false)
	, mIsDirty(true)
{

}

SGfx_Camera::~SGfx_Camera()
{

}

void SGfx_Camera::SetProjectionJitter(const SC_Vector2& aJitter) const
{
	mProjectionJitter = aJitter;
	mIsDirty = true;
}

const SC_Vector2& SGfx_Camera::GetProjectionJitter() const
{
	return mProjectionJitter;
}

void SGfx_Camera::SetPerspectiveProjection(const SC_Vector2& aSize, float aNear, float aFar, float aFov /*= 75.f*/)
{
	mProjection = SC_PerspectiveInvZMatrix(SC_Math::DegreesToRadians(aFov * 0.5f), aSize.x, aSize.y, aNear, aFar);
	mInvProjection = mProjection.Inverse();
	mSize = aSize;
	mNear = aNear;
	mFar = aFar;
	mFov = aFov;
	mAspectRatio = aSize.x / aSize.y;

	mIsOrthogonal = false;
	mIsDirty = true;
}

void SGfx_Camera::SetOrthogonalProjection(const SC_Vector2& aSize, float aNear, float aFar)
{
	float zScale = 1.0f / (aFar - aNear);
	float zOffset = -aNear;
	mProjection = SC_OrthogonalInvZMatrix(aSize.x * 0.5f, aSize.y * 0.5f, zScale, zOffset);
	mInvProjection = mProjection.Inverse();
	mSize = aSize;
	mNear = aNear;
	mFar = aFar;
	mFov = 0.f;

	mIsOrthogonal = true;
	mIsDirty = true;
}

void SGfx_Camera::SetViewportOffset(const SC_IntVector2& aOffset)
{
	mViewportOffset = aOffset;
}

void SGfx_Camera::Move(const SC_Vector& aDirection, float aLength)
{
	SC_Vector target(mTransform.GetPosition() + (aDirection * aLength));
	SetPosition(target);
}

void SGfx_Camera::Rotate(const SC_Vector& aRotationAngles)
{
	SC_Quaternion rotationQ(mTransform);

	SC_Quaternion pitch = SC_Quaternion::CreateRotation(GetRight(), SC_Math::DegreesToRadians(aRotationAngles.x));
	rotationQ *= pitch;

	SC_Quaternion yaw = SC_Quaternion::CreateRotation(SC_Vector::UpVector(), SC_Math::DegreesToRadians(aRotationAngles.y));
	rotationQ *= yaw;

	SC_Quaternion roll = SC_Quaternion::CreateRotation(GetForward(), SC_Math::DegreesToRadians(aRotationAngles.z));
	rotationQ *= roll;

	SC_Vector position = GetPosition();
	mTransform = rotationQ.AsMatrix();
	SetPosition(position);
}

void SGfx_Camera::Rotate(const SC_Vector& aAxis, float aValue)
{
	SC_Matrix rotation = SC_Matrix::CreateRotation(aAxis, aValue);
	SC_Vector position = GetPosition();
	mTransform = mTransform * rotation;
	SetPosition(position);
}

void SGfx_Camera::SetPosition(const SC_Vector& aPos)
{
	mTransform.SetPosition(aPos);
	mIsDirty = true;
}

SC_Vector SGfx_Camera::GetPosition() const
{
	return mTransform.GetPosition();
}

void SGfx_Camera::LookAt(const SC_Vector& aTargetPos, const SC_Vector& aUpVector)
{
	SC_LookAtMatrix lookAtMatrix(GetPosition(), aTargetPos, aUpVector);
	mTransform = lookAtMatrix.Inverse();
	mIsDirty = true;
}

SGfx_ViewConstants SGfx_Camera::GetViewConstants() const
{
	if (mIsDirty)
		Update();

	return mCachedConstants;
}

const SGfx_Frustum& SGfx_Camera::GetFrustum() const
{
	if (mIsDirty)
		Update();

	return mFrustum;
}

const SC_Vector& SGfx_Camera::GetForward() const
{
	if (mIsDirty)
		Update();

	return mForward;
}

const SC_Vector& SGfx_Camera::GetUp() const
{
	if (mIsDirty)
		Update();

	return mUp;
}
const SC_Vector& SGfx_Camera::GetRight() const
{
	if (mIsDirty)
		Update();

	return mRight;
}

const SC_Vector2& SGfx_Camera::GetSize() const
{
	return mSize;
}

float SGfx_Camera::GetNear() const
{
	return mNear;
}

float SGfx_Camera::GetFar() const
{
	return mFar;
}

void SGfx_Camera::GetCornersOnPlane(float aZ, SC_Vector* aOutCorners) const
{
	assert(aOutCorners != NULL);
	if (mIsDirty)
		Update();

	float xScale = 0.0f;
	float yScale = 0.0f;
	if (mIsOrthogonal)
	{
		xScale = mSize.x * 0.5f;
		yScale = mSize.y * 0.5f;
	}
	else
	{
		xScale = aZ * SC_Math::Tan(SC_Math::DegreesToRadians(mFov) * 0.5f);
		yScale = xScale / mAspectRatio;
	}

	const SC_Vector axisX(1.0f, 0.0f, 0.0f);
	const SC_Vector axisY(0.0f, 1.0f, 0.0f);
	const SC_Vector axisZ(0.0f, 0.0f, 1.0f);
	aOutCorners[0] = (axisZ * aZ + axisY * yScale + axisX * xScale) * mTransform;
	aOutCorners[1] = (axisZ * aZ + axisY * yScale - axisX * xScale) * mTransform;
	aOutCorners[2] = (axisZ * aZ - axisY * yScale + axisX * xScale) * mTransform;
	aOutCorners[3] = (axisZ * aZ - axisY * yScale - axisX * xScale) * mTransform;
}

void SGfx_Camera::Update() const
{
	const SC_Vector2 backbufferSize = SR_RenderDevice::gInstance->GetSwapChain()->GetProperties().mSize;

	SC_Matrix projectionWithJitter = mProjection;
	projectionWithJitter.m44[2][0] = mProjectionJitter.x;
	projectionWithJitter.m44[2][1] = mProjectionJitter.y;

	mCachedConstants.mWorldToCamera = mTransform.Inverse();
	mCachedConstants.mCameraToWorld = mTransform;

	mCachedConstants.mCameraToClip = projectionWithJitter;
	mCachedConstants.mClipToCamera = projectionWithJitter.Inverse();

	mCachedConstants.mWorldToClip = mCachedConstants.mWorldToCamera * projectionWithJitter;
	mCachedConstants.mClipToWorld = mCachedConstants.mClipToCamera * mCachedConstants.mCameraToWorld;

	mCachedConstants.mWorldToClip_NoJitter = mCachedConstants.mWorldToCamera * mProjection;

	SC_Vector2 viewportSize = mSize;
	viewportSize.x = SC_Max(viewportSize.x, 1.f);
	viewportSize.y = SC_Max(viewportSize.y, 1.f);
	SC_Vector2 pixelToClipScale = SC_Vector2(2, -2) / viewportSize;
	SC_Vector2 pixelToClipOffset = SC_Vector2(-1, 1) - SC_Vector2(0.0f, 0.0f) * pixelToClipScale;
	mCachedConstants.mPixelToClipScaleAndOffset = SC_Vector4(pixelToClipScale, pixelToClipOffset);

	mCachedConstants.mViewportPosAndInvSize = SC_Vector4((float)mViewportOffset.x, (float)mViewportOffset.y, 1.0f / mSize.x, 1.0f / mSize.y);
	mCachedConstants.mViewportSizeAndScale = SC_Vector4(mSize.x, mSize.y, mSize.x / backbufferSize.x, mSize.y / backbufferSize.y);

	mCachedConstants.mRenderTargetSizeAndInvSize = SC_Vector4(backbufferSize, SC_Vector2(1.0f / backbufferSize.x, 1.0f / backbufferSize.y));

	mCachedConstants.mCameraPosition = mTransform.GetPosition();
	mCachedConstants.mFov = mFov;

	mFrustum.Update(mCachedConstants.mWorldToClip);

	mRight.x = mCachedConstants.mCameraToWorld[0];
	mRight.y = mCachedConstants.mCameraToWorld[1];
	mRight.z = mCachedConstants.mCameraToWorld[2];
	mRight.Normalize();
	mUp.x = mCachedConstants.mCameraToWorld[4];
	mUp.y = mCachedConstants.mCameraToWorld[5];
	mUp.z = mCachedConstants.mCameraToWorld[6];
	mUp.Normalize();
	mForward.x = mCachedConstants.mCameraToWorld[8];
	mForward.y = mCachedConstants.mCameraToWorld[9];
	mForward.z = mCachedConstants.mCameraToWorld[10];
	mForward.Normalize();

	mIsDirty = false;
}
