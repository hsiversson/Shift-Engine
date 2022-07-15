#include "SED_TransformationGizmo.h"

#include "GameFramework/Entity/Components/SGF_TransformComponent.h"

#include "ImGuizmo/ImGuizmo.h"

static const ImU32 gDirectionColors[3] = { IM_COL32(170, 0, 0, 255), IM_COL32(0, 170, 0, 255), IM_COL32(0, 0, 170, 255) };
static const ImU32 gSelectionColor = IM_COL32(170, 170, 0, 255);
static const SC_Vector gGlobalAxis[3] = { SC_Vector::RightVector(), SC_Vector::UpVector(), SC_Vector::ForwardVector() };

//static float GetSegmentClipSpaceLength(const SC_Vector4& aP0, const SC_Vector4& aP1, const SC_Matrix& aMatrix, float aAspectRatio)
//{
//	SC_Vector4 start = aP0 * aMatrix;
//	if (SC_Math::Abs(start.w) > FLT_EPSILON)
//		start *= 1.f / start.w;
//
//	SC_Vector4 end = aP1 * aMatrix;
//	if (SC_Math::Abs(end.w) > FLT_EPSILON)
//		end *= 1.f / end.w;
//
//	SC_Vector4 clipSpaceAxis = end - start;
//	clipSpaceAxis.y /= aAspectRatio;
//	return clipSpaceAxis.Length();
//}

static ImVec2 MakeImVec2(const SC_Vector2& aVector)
{
	return ImVec2(aVector.x, aVector.y);
}

SED_TransformationGizmo::SED_TransformationGizmo()
	: mMode(SED_GizmoMode::Translate)
	, mSpace(SED_GizmoSpace::World)
	, mDrawList(nullptr)
{

}

SED_TransformationGizmo::~SED_TransformationGizmo()
{

}

void SED_TransformationGizmo::BeginFrame(const SC_Vector4& aBounds)
{
	ImGuizmo::BeginFrame();

	SC_Vector2 vpSize = aBounds.ZW() - aBounds.XY();
	mRect = { aBounds.x, aBounds.y, vpSize.x, vpSize.y };

	//const ImU32 flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
	//
	//
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
	//ImGui::PushStyleColor(ImGuiCol_Border, 0);
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	//
	//ImGui::SetNextWindowSize(MakeImVec2(vpSize));
	//ImGui::SetNextWindowPos(MakeImVec2(aBounds.XY()));
	//ImGui::Begin("gizmo", nullptr, flags);
	//mDrawList = ImGui::GetWindowDrawList();
	//ImGui::End();
	//
	//ImGui::PopStyleVar();
	//ImGui::PopStyleColor(2);
}

void SED_TransformationGizmo::SetViewAndProjection(const SC_Matrix& aView, const SC_Matrix& aProjection)
{
	mWorldToCamera = aView;
	mCameraToWorld = mWorldToCamera.Inverse();
	mCameraToClip = aProjection;
	mWorldToClip = mWorldToCamera * mCameraToClip;

	mCameraForward.x = mCameraToWorld[8];
	mCameraForward.y = mCameraToWorld[9];
	mCameraForward.z = mCameraToWorld[10];
	mCameraForward.Normalize();
}

void SED_TransformationGizmo::SetViewportPositionAndSize(const SC_Vector4& aPositionAndSize)
{
	mViewportPosAndSize = aPositionAndSize;
	//mAspectRatio = mViewportPosAndSize.z / mViewportPosAndSize.w;
}

bool SED_TransformationGizmo::Manipulate(bool /*aShouldSnap*/ /*= false*/, float /*aSnapValue*/ /*= 0.0f*/)
{
	if (mSelectedEntity.GetHandle() != SGF_InvalidEntityHandle)
	{
		SGF_TransformComponent* transformComponent = mSelectedEntity.GetComponent<SGF_TransformComponent>();
		SC_Matrix transform = transformComponent->GetTransform();

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(mViewportPosAndSize.x, mViewportPosAndSize.y, mViewportPosAndSize.z, mViewportPosAndSize.w);
		bool result = ImGuizmo::Manipulate(mWorldToCamera.m, mCameraToClip.m, ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, transform.m);

		transformComponent->SetTransform(transform);

		return result;
	}
	else
		return false;

	//mModel = aTransform;
	//mModelInv = mModel.Inverse();
	//mMVP = mModel * mWorldToClip;
	//
	//SC_Vector4 rightViewInverse = mCameraToWorld.mVectorX * mModelInv;
	//float rightLength = GetSegmentClipSpaceLength(SC_Vector4(0.f), rightViewInverse, mMVP, mAspectRatio);
	//mScreenFactor = 0.1f / rightLength;
	//
	//bool didManipulate = false;
	//if (mMode == SED_GizmoMode::Translate)
	//{
	//	didManipulate = HandleTranslation(aTransform, aShouldSnap, aSnapValue);
	//	DrawTranslationGizmo();
	//}
	//else if (mMode == SED_GizmoMode::Rotate)
	//{
	//	didManipulate = HandleRotation(aTransform, aShouldSnap, aSnapValue);
	//	DrawRotationGizmo();
	//}
	//else if (mMode == SED_GizmoMode::Scale)
	//{
	//	didManipulate = HandleScale(aTransform, aShouldSnap, aSnapValue);
	//	DrawScaleGizmo();
	//}

	//return didManipulate;
}

void SED_TransformationGizmo::SetSelectedEntity(const SGF_Entity& aEntity)
{
	mSelectedEntity = aEntity;
}

void SED_TransformationGizmo::ComputeTripodAxisAndVisibility(const uint32 /*aAxisIndex*/, SC_Vector& /*aDirAxis*/, SC_Vector& /*aDirPlaneX*/, SC_Vector& /*aDirPlaneY*/, bool& /*aUnderAxisLimit*/, bool& /*aUnderPlaneLimit*/, const bool /*aLocalCoordinates*/)
{
	//aDirAxis = gGlobalAxis[aAxisIndex];
	//aDirPlaneX = gGlobalAxis[(aAxisIndex + 1) % 3];
	//aDirPlaneY = gGlobalAxis[(aAxisIndex + 2) % 3];
	//
	//if (mIsActive && (gContext.mActualID == -1 || gContext.mActualID == gContext.mEditingID))
	//{
	//	// when using, use stored factors so the gizmo doesn't flip when we translate
	//	aUnderAxisLimit = gContext.mBelowAxisLimit[aAxisIndex];
	//	aUnderPlaneLimit = gContext.mBelowPlaneLimit[aAxisIndex];
	//
	//	aDirAxis *= gContext.mAxisFactor[aAxisIndex];
	//	aDirPlaneX *= gContext.mAxisFactor[(aAxisIndex + 1) % 3];
	//	aDirPlaneY *= gContext.mAxisFactor[(aAxisIndex + 2) % 3];
	//}
	//else
	//{
	//	// new method
	//	float lenDir = GetSegmentLengthClipSpace(makeVect(0.f, 0.f, 0.f), dirAxis, aLocalCoordinates);
	//	float lenDirMinus = GetSegmentLengthClipSpace(makeVect(0.f, 0.f, 0.f), -dirAxis, aLocalCoordinates);
	//
	//	float lenDirPlaneX = GetSegmentLengthClipSpace(makeVect(0.f, 0.f, 0.f), dirPlaneX, aLocalCoordinates);
	//	float lenDirMinusPlaneX = GetSegmentLengthClipSpace(makeVect(0.f, 0.f, 0.f), -dirPlaneX, aLocalCoordinates);
	//
	//	float lenDirPlaneY = GetSegmentLengthClipSpace(makeVect(0.f, 0.f, 0.f), dirPlaneY, aLocalCoordinates);
	//	float lenDirMinusPlaneY = GetSegmentLengthClipSpace(makeVect(0.f, 0.f, 0.f), -dirPlaneY, aLocalCoordinates);
	//
	//	// For readability
	//	float mulAxis = (lenDir < lenDirMinus&& SC_Math::Abs(lenDir - lenDirMinus) > FLT_EPSILON) ? -1.f : 1.f;
	//	float mulAxisX = (lenDirPlaneX < lenDirMinusPlaneX && SC_Math::Abs(lenDirPlaneX - lenDirMinusPlaneX) > FLT_EPSILON) ? -1.f : 1.f;
	//	float mulAxisY = (lenDirPlaneY < lenDirMinusPlaneY && SC_Math::Abs(lenDirPlaneY - lenDirMinusPlaneY) > FLT_EPSILON) ? -1.f : 1.f;
	//	dirAxis *= mulAxis;
	//	dirPlaneX *= mulAxisX;
	//	dirPlaneY *= mulAxisY;
	//
	//	// for axis
	//	float axisLengthInClipSpace = GetSegmentLengthClipSpace(makeVect(0.f, 0.f, 0.f), dirAxis * gContext.mScreenFactor, aLocalCoordinates);
	//	float paraSurf = GetParallelogram(makeVect(0.f, 0.f, 0.f), dirPlaneX * gContext.mScreenFactor, dirPlaneY * gContext.mScreenFactor);
	//	aUnderPlaneLimit = (paraSurf > 0.0025f);
	//	aUnderAxisLimit = (axisLengthInClipSpace > 0.02f);
	//
	//	// and store values
	//	gContext.mAxisFactor[aAxisIndex] = mulAxis;
	//	gContext.mAxisFactor[(aAxisIndex + 1) % 3] = mulAxisX;
	//	gContext.mAxisFactor[(aAxisIndex + 2) % 3] = mulAxisY;
	//	gContext.mBelowAxisLimit[aAxisIndex] = aUnderAxisLimit;
	//	gContext.mBelowPlaneLimit[aAxisIndex] = aUnderPlaneLimit;
	//}
}

bool SED_TransformationGizmo::HandleTranslation(SC_Matrix& /*aTransform*/, bool /*aShouldSnap*/ /*= false*/, float /*aSnapValue*/ /*= 0.0f*/)
{
	//if (!Intersects())
	//{
	//	return false;
	//}
	//
	//
	//const ImGuiIO& io = ImGui::GetIO();
	//const bool isLocal = aSpace == SED_GizmoSpace::Local;
	//bool modified = false;
	//
	//ImGui::CaptureMouseFromApp();
	//const float signedLength = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, gContext.mTranslationPlan);
	//const float len = SC_Math::Abs(signedLength); // near plan
	//const SC_Vector newPos = gContext.mRayOrigin + gContext.mRayVector * len;
	//
	//const SC_Vector newOrigin = newPos - gContext.mRelativeOrigin * gContext.mScreenFactor;
	//SC_Vector delta = newOrigin - gContext.mModel.v.position;
	//
	//
	//
	//return modified;

	ImGui::CaptureMouseFromApp();

	return false;
}

bool SED_TransformationGizmo::HandleRotation(SC_Matrix& /*aTransform*/, bool /*aShouldSnap*/ /*= false*/, float /*aSnapValue*/ /*= 0.0f*/)
{
	return true;
}

bool SED_TransformationGizmo::HandleScale(SC_Matrix& /*aTransform*/, bool /*aShouldSnap*/ /*= false*/, float /*aSnapValue*/ /*= 0.0f*/)
{
	return true;
}

void SED_TransformationGizmo::DrawTranslationGizmo()
{
	if (!mDrawList)
		return;

	//if (!Intersects())
	//	return;
	SC_Vector dirToCamera = mCameraToWorld.GetPosition() - mModel.GetPosition();
	if (dirToCamera.Dot(mCameraForward) >= 0.0f)
		return;

	float screenScale = dirToCamera.Length() * 0.1f;

	SC_Vector2 screenSpaceOrigin = WorldToScreen(mModel.GetPosition());

	for (uint32 i = 0; i < 3; ++i)
	{
		SC_Vector2 screenSpaceEnd = WorldToScreen(mModel.GetPosition() + gGlobalAxis[i] * screenScale);
		mDrawList->AddLine(MakeImVec2(screenSpaceOrigin), MakeImVec2(screenSpaceEnd), gDirectionColors[i], 3.f);

		SC_Vector2 dir = (screenSpaceOrigin - screenSpaceEnd).GetNormalized() * 6.0f;
		SC_Vector2 ortogonalDir(dir.y, -dir.x);
		SC_Vector2 a(screenSpaceEnd + dir);
		mDrawList->AddTriangleFilled(MakeImVec2(screenSpaceEnd - dir), MakeImVec2(a + ortogonalDir), MakeImVec2(a - ortogonalDir), gDirectionColors[i]);

		static constexpr float quadMin = 0.1f;
		static constexpr float quadMax = 0.4f;
		static constexpr float quadUV[8] = { quadMin, quadMin, quadMin, quadMax, quadMax, quadMax, quadMax, quadMin };
		ImVec2 screenQuadPts[4];
		SC_Vector dirPlaneX = gGlobalAxis[(i + 1) % 3];
		SC_Vector dirPlaneY = gGlobalAxis[(i + 2) % 3];
		for (int j = 0; j < 4; ++j)
		{
			SC_Vector cornerWorldPos = (dirPlaneX * quadUV[j * 2] + dirPlaneY * quadUV[j * 2 + 1]) * screenScale;
			screenQuadPts[j] = MakeImVec2(ModelToScreen(cornerWorldPos));
		}
		mDrawList->AddPolyline(screenQuadPts, 4, gDirectionColors[i], true, 2.0f);
		mDrawList->AddConvexPolyFilled(screenQuadPts, 4, gSelectionColor);
	}

	//const SC_Vector2 origin = worldToPos(mViewProjection, mModel.GetPosition());
	//
	//bool underAxisLimit = false;
	//bool underPlaneLimit = false;
	//for (uint32 i = 0; i < 3; ++i)
	//{
	//	SC_Vector dirPlaneX, dirPlaneY, dirAxis;
	//	ComputeTripodAxisAndVisibility(i, dirAxis, dirPlaneX, dirPlaneY, underAxisLimit, underPlaneLimit);
	//
	//	if (!mIsActive || (mIsActive && type == MT_MOVE_X + i))
	//	{
	//		if (underAxisLimit && Intersects(op, static_cast<OPERATION>(TRANSLATE_X << i)))
	//		{
	//			SC_Vector2 baseSSpace = worldToPos(dirAxis * 0.1f * gContext.mScreenFactor, gContext.mMVP);
	//			SC_Vector2 worldDirSSpace = worldToPos(dirAxis * gContext.mScreenFactor, gContext.mMVP);
	//
	//			drawList->AddLine(MakeImVec2(baseSSpace), MakeImVec2(worldDirSSpace), colors[i + 1], 3.f);
	//
	//			// Arrow head begin
	//			SC_Vector2 dir(origin - worldDirSSpace);
	//			dir.Normalize();
	//			dir *= 6.0f;
	//
	//			SC_Vector2 ortogonalDir(dir.y, -dir.x); // Perpendicular vector
	//			SC_Vector2 a(worldDirSSpace + dir);
	//			drawList->AddTriangleFilled(MakeImVec2(worldDirSSpace - dir), MakeImVec2(a + ortogonalDir), MakeImVec2(a - ortogonalDir), colors[i + 1]);
	//		}
	//	}
	//
	//	//if (!gContext.mbUsing || (gContext.mbUsing && type == MT_MOVE_YZ + i))
	//	//{
	//	//	if (belowPlaneLimit && Contains(op, TRANSLATE_PLANS[i]))
	//	//	{
	//	//		ImVec2 screenQuadPts[4];
	//	//		for (int j = 0; j < 4; ++j)
	//	//		{
	//	//			SC_Vector cornerWorldPos = (dirPlaneX * quadUV[j * 2] + dirPlaneY * quadUV[j * 2 + 1]) * gContext.mScreenFactor;
	//	//			screenQuadPts[j] = worldToPos(cornerWorldPos, gContext.mMVP);
	//	//		}
	//	//		drawList->AddPolyline(screenQuadPts, 4, gDirectionColors[i], true, 1.0f);
	//	//		drawList->AddConvexPolyFilled(screenQuadPts, 4, colors[i + 4]);
	//	//	}
	//	//}
	//}
}

void SED_TransformationGizmo::DrawRotationGizmo()
{

}

void SED_TransformationGizmo::DrawScaleGizmo()
{

}

SC_Vector2 SED_TransformationGizmo::WorldToScreen(const SC_Vector& aWorldPos) const
{
	SC_Vector4 clipPos = SC_Vector4(aWorldPos, 1.0f) * mWorldToClip;
	SC_Vector2 screenPos = (clipPos.XY() / clipPos.w) * 0.5f + 0.5f;
	screenPos.y = 1.0f - screenPos.y;
	screenPos *= mViewportPosAndSize.ZW();
	screenPos += mViewportPosAndSize.XY();
	return screenPos;
}

SC_Vector2 SED_TransformationGizmo::ModelToScreen(const SC_Vector& aWorldPos) const
{
	SC_Vector4 clipPos = SC_Vector4(aWorldPos, 1.0f) * mMVP;
	SC_Vector2 screenPos = (clipPos.XY() / clipPos.w) * 0.5f + 0.5f;
	screenPos.y = 1.0f - screenPos.y;
	screenPos *= mViewportPosAndSize.ZW();
	screenPos += mViewportPosAndSize.XY();
	return screenPos;
}

