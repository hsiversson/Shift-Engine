#include "SGfx_SceneGraph.h"
#include "SGfx_World.h"
#include "Graphics/Mesh/SGfx_Model.h"
#include "Graphics/Lighting/SGfx_Light.h"
#include "Graphics/Particles/SGfx_ParticleEffect.h"
#include "Graphics/Decals/SGfx_Decal.h"
#include "Graphics/View/SGfx_Camera.h"
#include "Graphics/View/SGfx_View.h"
#include "Graphics/Mesh/SGfx_MeshInstance.h"
#include "Graphics/Lighting/Shadows/SGfx_ShadowSystem.h"
#include "Graphics/Renderer/SGfx_Renderer.h"

SGfx_SceneGraph::SGfx_SceneGraph(SGfx_World* aParentWorld)
	: mParentWorld(aParentWorld)
{

}

SGfx_SceneGraph::~SGfx_SceneGraph()
{

}

void SGfx_SceneGraph::PrepareView(SGfx_View* aView)
{
	SGfx_ViewData& prepareData = aView->GetPrepareData();

	//SC_Array<SC_Future<bool>> shadowCullingTasks;
	if (aView->IsMainView())
	{
		SGfx_CascadedShadowMap* csm = mParentWorld->GetRenderer()->GetShadowMapSystem()->GetCSM();
		const uint32 numCascades = csm->GetSettings().gNumCascades;

		prepareData.mCSMViews.Respace(numCascades);
		for (uint32 i = 0; i < numCascades; ++i)
		{
			SC_Ref<SGfx_View>& cascadeView = prepareData.mCSMViews[i];
			if (!cascadeView)
			{
				cascadeView = SC_MakeRef<SGfx_View>();
				cascadeView->SetOnlyDepth(true);
			}

			cascadeView->SetCamera(csm->GetCascade(i).mCamera);

			//SGfx_View* view = prepareData.mCSMViews[i].get();
			//auto task = [this, view]()
			//{
			//	CullMeshes(view);
			//};
			//shadowCullingTasks.Add(SC_ThreadPool::Get().SubmitTask(task));

			CullMeshes(cascadeView.get());
		}
	}

	// Prepare Tasks (async?)
	CullLights(aView);
	CullMeshes(aView);

	//AddRaytracingGeometry(aView);

	//for (SC_Future<bool>& task : shadowCullingTasks)
	//	task.Wait();
}

void SGfx_SceneGraph::AddModel(SC_Ref<SGfx_Model> aModel)
{
	const SC_Array<SC_Ref<SGfx_MeshInstance>>& meshes = aModel->GetMeshes();

	SC_MutexLock lock(mMeshesMutex);
	mMeshes.Add(meshes);
}

void SGfx_SceneGraph::RemoveModel(SC_Ref<SGfx_Model> aModel)
{
	const SC_Array<SC_Ref<SGfx_MeshInstance>>& meshes = aModel->GetMeshes();

	SC_MutexLock lock(mMeshesMutex);
	for (const SC_Ref<SGfx_MeshInstance>& mesh : meshes)
		mMeshes.RemoveCyclic(mesh);
}

void SGfx_SceneGraph::AddMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance)
{
	SC_MutexLock lock(mMeshesMutex);
	mMeshes.Add(aMeshInstance);
}

void SGfx_SceneGraph::RemoveMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance)
{
	SC_MutexLock lock(mMeshesMutex);
	mMeshes.RemoveCyclic(aMeshInstance);
}

void SGfx_SceneGraph::AddLight(SC_Ref<SGfx_Light> aLight)
{
	SC_MutexLock lock(mLightsMutex);
	mLights.Add(aLight);
}

void SGfx_SceneGraph::RemoveLight(SC_Ref<SGfx_Light> aLight)
{
	SC_MutexLock lock(mLightsMutex);
	mLights.RemoveCyclic(aLight);
}

void SGfx_SceneGraph::AddParticleEffect(SC_Ref<SGfx_ParticleEffect> aParticleEffect)
{
	SC_MutexLock lock(mParticlesMutex);
	mParticleEffects.Add(aParticleEffect);
}

void SGfx_SceneGraph::RemoveParticleEffect(SC_Ref<SGfx_ParticleEffect> aParticleEffect)
{
	SC_MutexLock lock(mParticlesMutex);
	mParticleEffects.RemoveCyclic(aParticleEffect);
}

void SGfx_SceneGraph::AddDecal(SC_Ref<SGfx_Decal> aDecal)
{
	SC_MutexLock lock(mDecalsMutex);
	mDecals.Add(aDecal);
}

void SGfx_SceneGraph::RemoveDecal(SC_Ref<SGfx_Decal> aDecal)
{
	SC_MutexLock lock(mDecalsMutex);
	mDecals.RemoveCyclic(aDecal);
}

void SGfx_SceneGraph::CullMeshes(SGfx_View* aView)
{
	SC_Array<SC_Ref<SGfx_MeshInstance>> tempMeshList; // Hybrid array?
	{
		SC_MutexLock lock(mMeshesMutex);
		tempMeshList.Add(mMeshes);
	}

	SGfx_ViewData& prepareData = aView->GetPrepareData();
	const SGfx_Camera& camera = aView->GetCamera();
	const SGfx_Frustum& frustum = camera.GetFrustum();

	const bool depthOnly = aView->IsOnlyDepth();

	for (const SC_Ref<SGfx_MeshInstance>& mesh : tempMeshList)
	{
		if (!mesh)
			continue;

		if (!mesh->GetMaterialInstance())
			continue;

		const SC_AABB& boundingBox = mesh->GetBoundingBox();
		const SC_Sphere boundingSphere(boundingBox);
		const float distanceToCamera = (camera.GetPosition() - boundingBox.GetCenter()).Length();

#if ENABLE_RAYTRACING
		if (!depthOnly && mesh->IncludeInRaytracingScene() && distanceToCamera < 200.0f) // TODO: FIX THIS CULLING
		{
			prepareData.mRaytracingInstances.Add(mesh->GetRaytracingData());
		}
#endif

		bool isVisible = frustum.Intersects(boundingSphere);
		if (!isVisible)
			continue;

		isVisible = frustum.Intersects(mesh->GetBoundingBox());
		if (!isVisible)
			continue;

		// HZB check for occlusion culling

		if (isVisible)
		{
			SGfx_Mesh* meshTemplate = mesh->GetMeshTemplate();

			// Add to render queues because this mesh is visible inside our current view
			SGfx_RenderObject renderObject;

			if (SR_RenderDevice::gInstance->GetSupportCaps().mEnableMeshShaders)
			{
				renderObject.mVertexBuffer = meshTemplate->GetVertexBuffer();
				renderObject.mMeshletBuffer = meshTemplate->GetMeshletBuffer();
				renderObject.mVertexIndexBuffer = meshTemplate->GetVertexIndexBuffer();
				renderObject.mPrimitiveIndexBuffer = meshTemplate->GetPrimitiveIndexBuffer();
			}
			else
			{
				renderObject.mVertexBufferResource = meshTemplate->GetVertexBufferResource();
				renderObject.mIndexBufferResource = meshTemplate->GetIndexBufferResource();
			}


			renderObject.mTransform = mesh->GetTransform();
			renderObject.mPrevTransform = mesh->GetPrevTransform();

			renderObject.mMaterialIndex = mesh->GetMaterialInstance()->GetMaterialIndex();
			renderObject.mOutputVelocity = mesh->GetMaterialInstance()->GetMaterialTemplate()->OutputVelocity();

			renderObject.mSortDistance = distanceToCamera;

			SR_ShaderState* depthShader = mesh->GetMaterialInstance()->GetMaterialTemplate()->GetShaderState(meshTemplate->GetVertexLayout(), (depthOnly) ? SGfx_MaterialShaderType::ShadowDepth : SGfx_MaterialShaderType::Depth);
			SR_ShaderState* defaultShader = mesh->GetMaterialInstance()->GetMaterialTemplate()->GetShaderState(meshTemplate->GetVertexLayout(), SGfx_MaterialShaderType::Default);
			if (!depthShader || (!depthOnly && !defaultShader))
				continue;

			renderObject.mShader = depthShader;
			prepareData.mDepthQueue.Add(renderObject);

			if (!depthOnly)
			{
				renderObject.mShader = defaultShader;
				prepareData.mOpaqueQueue.Add(renderObject);
			}
		}
	}
}

void SGfx_SceneGraph::CullLights(SGfx_View* aView)
{
	SC_Array<SC_Ref<SGfx_Light>> tempLightsList; // Hybrid array?
	{
		SC_MutexLock lock(mLightsMutex);
		tempLightsList.Add(mLights);
	}

	const SGfx_Camera& camera = aView->GetCamera();
	const SGfx_Frustum& frustum = camera.GetFrustum();

	for (const SC_Ref<SGfx_Light>& light : tempLightsList)
	{
		if (light->GetType() == SGfx_LightType::Directional || frustum.Intersects(light->GetBoundingSphere()))
		{
			// Add to light queues because this light is visible inside our current view
		}
	}
}

void SGfx_SceneGraph::AddRaytracingGeometry(SGfx_View* /*aView*/)
{
	SC_Array<SC_Ref<SGfx_MeshInstance>> tempMeshList; // Hybrid array?
	{
		SC_MutexLock lock(mMeshesMutex);
		tempMeshList.Add(mMeshes);
	}

	//SGfx_ViewData& prepareData = aView->GetPrepareData();


}
