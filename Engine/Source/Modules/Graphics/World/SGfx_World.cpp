#include "SGfx_World.h"
#include "SGfx_SceneGraph.h"
#include "View/SGfx_View.h"
#include "Environment/SGfx_Environment.h"
#include "Material/SGfx_MaterialCache.h"
#include "Renderer/SGfx_Renderer.h"
#include "Lighting/Shadows/SGfx_ShadowSystem.h"

SGfx_World::SGfx_World()
{
	mSceneGraph = SC_MakeUnique<SGfx_SceneGraph>(this);
	mEnvironment = SC_MakeUnique<SGfx_Environment>();

	mRenderer = SC_MakeUnique<SGfx_Renderer>();
	if (!mRenderer->Init())
	{
		assert(false);
		return;
	}

}

SGfx_World::~SGfx_World()
{

}

SC_Ref<SGfx_View> SGfx_World::CreateView()
{
	SC_Ref<SGfx_View> view = SC_MakeRef<SGfx_View>();
	mViews.Add(view);
	return view;
}

void SGfx_World::DestroyView(const SC_Ref<SGfx_View>& aView)
{
	int32 idx = mViews.Find(aView);
	if (idx != mViews.gFindResultNone)
	{
		mViews.RemoveCyclicAt(idx);
	}
}

void SGfx_World::PrepareView(SGfx_View* aView)
{
	SGfx_MaterialGPUDataBuffer& materialGpuBuffer = SGfx_MaterialGPUDataBuffer::Get();
	materialGpuBuffer.UpdateBuffer();

	// Begin Prepare
	SGfx_ViewData& prepareData = aView->GetPrepareData();
	const SGfx_ViewConstants prevViewConstants = prepareData.mSceneConstants.mViewConstants;

	// Set Generic data such as view constants etc
	prepareData.Clear();

	SC_Vector2 jitter(0.0f);
	if (mRenderer->GetSettings().mEnableTemporalAA)
	{
		//SC_Vector2 jitter = mRenderer->GetJitter({ (int32)aView->GetCamera().GetSize().x, (int32)aView->GetCamera().GetSize().y });
		jitter = mRenderer->GetJitter(mRenderer->GetScreenColor()->GetResource()->GetProperties().mSize.XY());
	}

	aView->GetCamera().SetProjectionJitter(jitter);

	prepareData.mSceneConstants.mFrameIndex = SC_Time::gFrameCounter;
	prepareData.mSceneConstants.mFrameTimeDelta = SC_Time::gDeltaTime;
	prepareData.mSceneConstants.mViewConstants = aView->GetCamera().GetViewConstants();

	// Set previous view matrices for reprojections
	prepareData.mSceneConstants.mViewConstants.mPrevWorldToClip = prevViewConstants.mWorldToClip;
	prepareData.mSceneConstants.mViewConstants.mPrevWorldToClip_NoJitter = prevViewConstants.mWorldToClip_NoJitter;
	prepareData.mSceneConstants.mViewConstants.mPrevClipToWorld = prevViewConstants.mClipToWorld;
	prepareData.mSceneConstants.mViewConstants.mPrevWorldToCamera = prevViewConstants.mWorldToCamera;
	prepareData.mSceneConstants.mViewConstants.mPrevCameraToWorld = prevViewConstants.mCameraToWorld;
	prepareData.mSceneConstants.mViewConstants.mPrevCameraToClip = prevViewConstants.mCameraToClip;
	prepareData.mSceneConstants.mViewConstants.mPrevClipToCamera = prevViewConstants.mClipToCamera;

	prepareData.mSceneConstants.mEnvironmentConstants = mEnvironment->GetConstants();

	mRenderer->GetShadowMapSystem()->GetCSM()->UpdateViews(aView);
	prepareData.mSceneConstants.mShadowConstants = mRenderer->GetShadowMapSystem()->GetShadowConstants();

	mSceneGraph->PrepareView(aView);

	prepareData.mSkybox = mEnvironment->GetSkybox();
}

void SGfx_World::RenderView(SGfx_View* aView)
{
	// Choose renderer?
	mRenderer->RenderView(aView);

}

void SGfx_World::AddModel(SC_Ref<SGfx_Model> aModel)
{
	mSceneGraph->AddModel(aModel);
}

void SGfx_World::RemoveModel(SC_Ref<SGfx_Model> aModel)
{
	mSceneGraph->RemoveModel(aModel);
}

void SGfx_World::AddMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance)
{
	mSceneGraph->AddMesh(aMeshInstance);
}

void SGfx_World::RemoveMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance)
{
	mSceneGraph->RemoveMesh(aMeshInstance);
}

void SGfx_World::AddLight(SC_Ref<SGfx_Light> aLight)
{
	mSceneGraph->AddLight(aLight);
}

void SGfx_World::RemoveLight(SC_Ref<SGfx_Light> aLight)
{
	mSceneGraph->RemoveLight(aLight);
}

void SGfx_World::AddParticleEffect(SC_Ref<SGfx_ParticleEffect> aParticleEffect)
{
	mSceneGraph->AddParticleEffect(aParticleEffect);
}

void SGfx_World::RemoveParticleEffect(SC_Ref<SGfx_ParticleEffect> aParticleEffect)
{
	mSceneGraph->RemoveParticleEffect(aParticleEffect);
}

void SGfx_World::AddDecal(SC_Ref<SGfx_Decal> aDecal)
{
	mSceneGraph->AddDecal(aDecal);
}

void SGfx_World::RemoveDecal(SC_Ref<SGfx_Decal> aDecal)
{
	mSceneGraph->RemoveDecal(aDecal);
}

SGfx_SceneGraph* SGfx_World::GetSceneGraph() const
{
	return mSceneGraph.get();
}

SGfx_Renderer* SGfx_World::GetRenderer() const
{
	return mRenderer.get();
}