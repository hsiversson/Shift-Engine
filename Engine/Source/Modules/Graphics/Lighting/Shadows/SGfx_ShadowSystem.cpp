#include "SGfx_ShadowSystem.h"
#include "Graphics/View/SGfx_Camera.h"
#include "Graphics/View/SGfx_View.h"
#include "Graphics/Renderer/SGfx_DrawInfo.h"
#include "Graphics/Renderer/SGfx_PrimitiveRenderer.h"

SGfx_CascadedShadowMap::SGfx_CascadedShadowMap()
#if IS_DEBUG_BUILD
	: mWasLocked(false)
#endif
{

}

SGfx_CascadedShadowMap::~SGfx_CascadedShadowMap()
{

}

bool SGfx_CascadedShadowMap::Init(const Settings& aSettings)
{
	mSettings = aSettings;

	if (!CreateTargets())
		return false;

	SR_BufferResourceProperties cbDesc;
	cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
	cbDesc.mElementCount = 1;
	cbDesc.mElementSize = sizeof(SGfx_SceneConstants);
	for (uint32 i = 0; i < mSettings.gNumCascades; ++i)
		mSceneConstantsBuffer[i] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);

	return true;
}

SC_Sphere GetMinimumBoundingSphereFromPoints(SC_Vector* aPoints, uint32 aNumPoints)
{
	SC_Sphere s;
	SC_AABB aabb;
	aabb.FromPoints(aPoints, aNumPoints);
	s.mCenter = (aabb.mMax - aabb.mMin) * 0.5f;

	s.mRadius = SC_FLT_LOWEST;
	for (uint32 i = 0; i < aNumPoints; ++i)
	{
		float l = (aPoints[i] - s.mCenter).Length2();
		if (l > s.mRadius)
			s.mRadius = l;
	}

	s.mRadius = SC_Math::Sqrt(s.mRadius);
	return s;
}

#if IS_DEBUG_BUILD
static const SC_Vector4& GetCascadeDebugColor(uint32 aCascadeIndex)
{
	static SC_Vector4 gColors[4] =
	{
		SC_Vector4(1.0f, 0.0f, 0.0f, 1.0f),
		SC_Vector4(1.0f, 1.0f, 0.0f, 1.0f),
		SC_Vector4(0.0f, 1.0f, 0.0f, 1.0f),
		SC_Vector4(0.0f, 0.0f, 1.0f, 1.0f)
	};

	return gColors[aCascadeIndex];
}
#endif

void SGfx_CascadedShadowMap::UpdateViews(SGfx_View* aMainView)
{
	const SGfx_ViewData& prepareData = aMainView->GetPrepareData();

#if IS_DEBUG_BUILD
	if (mSettings.mLockShadowView)
	{
		if (!mWasLocked)
		{
			mLockedViewCamera = aMainView->GetCamera();
			mWasLocked = true;
		}
	}
	else
		mWasLocked = false;

	const SGfx_Camera& camera = (mSettings.mLockShadowView) ? mLockedViewCamera : aMainView->GetCamera();
#else
	const SGfx_Camera& camera = aMainView->GetCamera();
#endif

	if (mSettings.mResolution != (uint32)mCascades[0].mTarget->GetResource()->GetProperties().mSize.x)
	{
		CreateTargets();
	}

	// Split Points
	float mainViewNear = camera.GetNear();
	float mainViewFar = SC_Min(camera.GetFar(), mSettings.mMaxDistance);
	CalculateSplitPoints(mainViewNear, mainViewFar);

	for (uint32 i = 0; i < mSettings.gNumCascades; ++i)
	{
		Cascade& cascade = mCascades[i];
		if (!cascade.mNeedsCacheUpdate)
			continue;

		const float cascadeNearZ = mSplitPoints[i] - mSplitPoints[i + 1] * 0.08f;
		const float cascadeFarZ = mSplitPoints[i + 1];

		SC_Vector frustumCorners[8];
		camera.GetCornersOnPlane(cascadeNearZ, frustumCorners);
		camera.GetCornersOnPlane(cascadeFarZ, frustumCorners + 4);

		SC_Sphere minBoundingSphere = GetMinimumBoundingSphereFromPoints(frustumCorners, 8);

		SC_Vector cameraToMBSCenter = minBoundingSphere.mCenter - camera.GetPosition();
		cascade.mCameraToMBSCenterCache =
		{
			cameraToMBSCenter.Dot(camera.GetRight()),
			cameraToMBSCenter.Dot(camera.GetUp()),
			cameraToMBSCenter.Dot(camera.GetForward())
		};
		cascade.mMBSCache = minBoundingSphere;
	}

	const SC_Vector& lightDir = prepareData.mSceneConstants.mEnvironmentConstants.mSunLightDirection;
	SC_Vector3f up = (lightDir.Dot(up) < -0.999f) ? SC_Vector3f(1.0f, 0.0f, 0.0f) : SC_Vector3f(0.0f, 1.0f, 0.0f);
	const uint32 lastCascadeIndex = mSettings.gNumCascades - 1;
	for (uint32 i = 0; i < mSettings.gNumCascades; ++i)
	{
		Cascade& cascade = mCascades[i];
		if (!cascade.mNeedsCacheUpdate)
			continue;

		const float lastRadius = mCascades[lastCascadeIndex].mMBSCache.mRadius;
		const float lastSplitPoint = mSplitPoints[lastCascadeIndex] - mSplitPoints[i + 1] * 0.08f;
		const float pushBackDistance = (lastRadius + lastSplitPoint) * 0.2f;

		float radius = mCascades[i].mMBSCache.mRadius;
		float size = radius * 2.4f;
		cascade.mCamera.SetOrthogonalProjection({ size, size }, -pushBackDistance, radius + mSettings.mMaxDistance);

		SC_Vector center(
			camera.GetPosition() +
			camera.GetRight()	* cascade.mCameraToMBSCenterCache.x +
			camera.GetUp()		* cascade.mCameraToMBSCenterCache.y +
			camera.GetForward() * cascade.mCameraToMBSCenterCache.z);

		const float metersPerSMTexel = size / mSettings.mResolution;
		SC_Vector3f& oldPosition = cascade.mOldPosition;
		float xerror = (center.x - oldPosition.x) / metersPerSMTexel;
		float yerror = (center.y - oldPosition.y) / metersPerSMTexel;
		float zerror = (center.z - oldPosition.z) / (radius * 0.05f);
		bool posChange = SC_Max(SC_Math::Abs(xerror), SC_Math::Abs(yerror), SC_Math::Abs(zerror)) > 1.0f;
		if (posChange)
		{
			center.x = oldPosition.x + SC_Math::Floor(xerror + 0.5f) * metersPerSMTexel;
			center.y = oldPosition.y + SC_Math::Floor(yerror + 0.5f) * metersPerSMTexel;
			center.z = oldPosition.z + SC_Math::Floor(zerror + 0.5f) * radius * 0.05f;
			oldPosition = center;

			const SC_Vector3f lightCameraPosition = cascade.mCamera.GetRight() * center.x + cascade.mCamera.GetUp() * center.y + cascade.mCamera.GetForward() * center.z;
			const SC_Vector3f lightCameraTarget = lightCameraPosition + -lightDir;

			cascade.mCamera.SetPosition(lightCameraPosition);
			cascade.mCamera.LookAt(lightCameraTarget, up);
		}
		//mCascades[i].mNeedsCacheUpdate = false;

#if IS_DEBUG_BUILD
		if (mSettings.mDebugDrawFrustums)
		{
			SC_Vector corners[8];
			cascade.mCamera.GetCornersOnPlane(cascade.mCamera.GetNear(), corners);
			cascade.mCamera.GetCornersOnPlane(cascade.mCamera.GetFar(), corners + 4);

			SC_Vector4 color = GetCascadeDebugColor(i);

			DRAW_LINE_COLORED(corners[0], corners[1], color);
			DRAW_LINE_COLORED(corners[1], corners[3], color);
			DRAW_LINE_COLORED(corners[3], corners[2], color);
			DRAW_LINE_COLORED(corners[2], corners[0], color);
			DRAW_LINE_COLORED(corners[0], corners[4], color);
			DRAW_LINE_COLORED(corners[4], corners[5], color);
			DRAW_LINE_COLORED(corners[5], corners[1], color);
			DRAW_LINE_COLORED(corners[1], corners[5], color);
			DRAW_LINE_COLORED(corners[5], corners[7], color);
			DRAW_LINE_COLORED(corners[7], corners[3], color);
			DRAW_LINE_COLORED(corners[3], corners[7], color);
			DRAW_LINE_COLORED(corners[7], corners[6], color);
			DRAW_LINE_COLORED(corners[6], corners[2], color);
			DRAW_LINE_COLORED(corners[2], corners[6], color);
			DRAW_LINE_COLORED(corners[6], corners[4], color);
		}
#endif
	}

}

void SGfx_CascadedShadowMap::Generate(SR_CommandList* aCmdList, const SGfx_ViewData& aRenderData)
{
	aCmdList->BeginEvent("Generate CSM");

	aCmdList->TransitionBarrier(SR_ResourceState_DepthWrite, mCSMResource);

	for (uint32 i = 0; i < mSettings.gNumCascades; ++i)
		RenderCascade(aCmdList, i, aRenderData.mCSMViews[i]->GetRenderData());

	aCmdList->TransitionBarrier(SR_ResourceState_DepthRead, mCSMResource);

	aCmdList->EndEvent(); // Generate CSM
}

const SGfx_CascadedShadowMap::Cascade& SGfx_CascadedShadowMap::GetCascade(uint32 aIndex) const
{
	return mCascades[aIndex];
}

SGfx_CascadedShadowMap::Settings& SGfx_CascadedShadowMap::GetSettings()
{
	return mSettings;
}

const SGfx_CascadedShadowMap::Settings& SGfx_CascadedShadowMap::GetSettings() const
{
	return mSettings;
}

SC_Vector4 SGfx_CascadedShadowMap::GetSplitPoints() const
{
	return SC_Vector4(mSplitPoints[1], mSplitPoints[2], mSplitPoints[3], mSplitPoints[4]);
}

const SC_IntVector4 SGfx_CascadedShadowMap::GetCSMDescriptorIndices() const
{
	uint32 i = 0;
	SC_IntVector4 indices;
	for (const auto& cascade : mCascades)
		indices[i++] = cascade.mTexture->GetDescriptorHeapIndex();

	return indices;
}

bool SGfx_CascadedShadowMap::InitProjections()
{
	return false;
}

bool SGfx_CascadedShadowMap::CreateTargets()
{
	SR_TextureResourceProperties textureResourceProperties;
	textureResourceProperties.mAllowDepthStencil = true;
	textureResourceProperties.mArraySize = mSettings.gNumCascades;
	textureResourceProperties.mSize = SC_IntVector(mSettings.mResolution, mSettings.mResolution, 0);
	textureResourceProperties.mFormat = SR_Format::D32_FLOAT;
	textureResourceProperties.mType = SR_ResourceType::Texture2D;
	textureResourceProperties.mNumMips = 1;
	textureResourceProperties.mDebugName = "Cascaded Shadow Map";
	mCSMResource = SR_RenderDevice::gInstance->CreateTextureResource(textureResourceProperties);

	mCascades.Respace(mSettings.gNumCascades);
	for (uint32 i = 0; i < mSettings.gNumCascades; ++i)
	{
		SR_DepthStencilProperties props(textureResourceProperties.mFormat);
		props.mFirstArrayIndex = (uint16)i;
		mCascades[i].mTarget = SR_RenderDevice::gInstance->CreateDepthStencil(props, mCSMResource);

		SR_TextureProperties texProps(textureResourceProperties.mFormat);
		texProps.mFirstArrayIndex = (uint16)i;
		mCascades[i].mTexture = SR_RenderDevice::gInstance->CreateTexture(texProps, mCSMResource);
	}

	return true;
}

void SGfx_CascadedShadowMap::RenderCascade(SR_CommandList* aCmdList, uint32 aIndex, const SGfx_ViewData& aRenderData)
{
	Cascade& cascade = mCascades[aIndex];

	std::string eventName("Cascade" + std::to_string(aIndex));
	aCmdList->BeginEvent(eventName.c_str());

	aCmdList->ClearDepthStencil(cascade.mTarget);
	aCmdList->SetRenderTargets(0, nullptr, cascade.mTarget);
	aCmdList->SetViewport(SR_Rect{ 0,0, mSettings.mResolution, mSettings.mResolution });
	aCmdList->SetScissorRect(SR_Rect{ 0,0, mSettings.mResolution, mSettings.mResolution });

	SGfx_SceneConstants sceneConstants(aRenderData.mSceneConstants);
	sceneConstants.mViewConstants = cascade.mCamera.GetViewConstants();
	mSceneConstantsBuffer[aIndex]->UpdateData(0, &sceneConstants, sizeof(SGfx_SceneConstants));
	aCmdList->SetRootConstantBuffer(mSceneConstantsBuffer[aIndex], 1);

	//uint32 i = 0;
	//mDrawInfoBuffers[aIndex].Respace(aRenderData.mDepthQueue.Count());
	//for (const SGfx_RenderObject& renderObj : aRenderData.mDepthQueue)
	//{
	//	if (!mDrawInfoBuffers[aIndex][i])
	//	{
	//		SR_BufferResourceProperties cbDesc;
	//		cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
	//		cbDesc.mElementCount = 1;
	//		cbDesc.mElementSize = sizeof(SGfx_MeshShadingDrawInfoStruct);
	//		mDrawInfoBuffers[aIndex][i] = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
	//	}
	//
	//	//SGfx_MeshShadingDrawInfoStruct drawInfo;
	//	//drawInfo.mTransform = renderObj.mTransform;
	//	//drawInfo.mPrevTransform = renderObj.mPrevTransform;
	//	//drawInfo.mVertexBufferDescriptorIndex = renderObj.mVertexBuffer->GetDescriptorHeapIndex();
	//	//drawInfo.mMeshletBufferDescriptorIndex = renderObj.mMeshletBuffer->GetDescriptorHeapIndex();
	//	//drawInfo.mVertexIndexBufferDescriptorIndex = renderObj.mVertexIndexBuffer->GetDescriptorHeapIndex();
	//	//drawInfo.mPrimitiveIndexBufferDescriptorIndex = renderObj.mPrimitiveIndexBuffer->GetDescriptorHeapIndex();
	//	//drawInfo.mMaterialIndex = renderObj.mMaterialIndex;
	//	//mDrawInfoBuffers[aIndex][i]->UpdateData(0, &drawInfo, sizeof(SGfx_MeshShadingDrawInfoStruct));
	//	//aCmdList->SetRootConstantBuffer(mDrawInfoBuffers[aIndex][i].get(), 0);
	//	//
	//	//aCmdList->SetShaderState(renderObj.mShader);
	//	//uint32 groupCount = renderObj.mMeshletBuffer->GetProperties().mElementCount;
	//	//aCmdList->DispatchMesh(groupCount);
	//	++i;
	//}

	aCmdList->EndEvent();

}

void SGfx_CascadedShadowMap::CalculateSplitPoints(float aNear, float aFar)
{
	const float splitIncrease = 1.0f / float(mSettings.gNumCascades);
	float splitPoint = 0.0f;
	for (int i = 0; i <= mSettings.gNumCascades; ++i)
	{
		const float logarithmicSplit = SC_Math::Pow(aFar - aNear + 1.0f, splitPoint) + aNear - 1.0f;
		const float linearSplit = aNear + (aFar - aNear) * splitPoint;
		mSplitPoints[i] = logarithmicSplit * mSettings.mSplitFactor + linearSplit * (1.0f - mSettings.mSplitFactor);
		splitPoint += splitIncrease;
	}
}

SGfx_ShadowSystem::SGfx_ShadowSystem()
{
	mCSM = SC_MakeUnique<SGfx_CascadedShadowMap>();
}

SGfx_ShadowSystem::~SGfx_ShadowSystem()
{

}

SC_Ref<SGfx_View> SGfx_ShadowSystem::CreateShadowView()
{
	return SC_Ref<SGfx_View>();
}

void SGfx_ShadowSystem::GenerateShadowMaps(const SGfx_ViewData& /*aRenderData*/)
{
}

SGfx_CascadedShadowMap* SGfx_ShadowSystem::GetCSM() const
{
	return mCSM.get();
}

SGfx_ShadowConstants SGfx_ShadowSystem::GetShadowConstants() const
{
	SGfx_ShadowConstants constants;

	const SGfx_CascadedShadowMap::Settings& csmSettings = mCSM->GetSettings();

	for (uint32 i = 0; i < csmSettings.gNumCascades; ++i)
	{
		const SC_Matrix biasMatrix(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 0.0f);

		constants.mCSMWorldToClip[i] = (mCSM->GetCascade(i).mCamera.GetViewConstants().mWorldToClip * biasMatrix);
	}

	constants.mCSMSplitPoints = mCSM->GetSplitPoints();
	constants.mCSMDescriptorIndices = mCSM->GetCSMDescriptorIndices();

	float csmResolution = (float)csmSettings.mResolution;
	constants.mCSMResolutionAndInv = SC_Vector2(csmResolution, 1.0f / csmResolution);

	return constants;
}
