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

void SGfx_SceneGraph::Update()
{
	FlushPendingActions();
}

void SGfx_SceneGraph::PrepareView(SGfx_View* aView)
{
	SC_PROFILER_FUNCTION();
	SGfx_ViewData& prepareData = aView->GetPrepareData();

	//SC_Array<SC_Future<bool>> shadowCullingTasks;
	//if (aView->IsMainView())
	//{
	//	SGfx_CascadedShadowMap* csm = mParentWorld->GetRenderer()->GetShadowMapSystem()->GetCSM();
	//	const uint32 numCascades = csm->GetSettings().gNumCascades;
	//
	//	prepareData.mCSMViews.Respace(numCascades);
	//	for (uint32 i = 0; i < numCascades; ++i)
	//	{
	//		SC_Ref<SGfx_View>& cascadeView = prepareData.mCSMViews[i];
	//		if (!cascadeView)
	//		{
	//			cascadeView = SC_MakeRef<SGfx_View>();
	//			cascadeView->SetOnlyDepth(true);
	//		}
	//
	//		cascadeView->SetCamera(csm->GetCascade(i).mCamera);
	//
	//		//SGfx_View* view = prepareData.mCSMViews[i].get();
	//		//auto task = [this, view]()
	//		//{
	//		//	CullMeshes(view);
	//		//};
	//		//shadowCullingTasks.Add(SC_ThreadPool::Get().SubmitTask(task));
	//
	//		CullMeshes(cascadeView.get());
	//	}
	//}

	prepareData.mPrepareCullMeshesEvent = SC_ThreadPool::Get().SubmitTask([this, aView]() { CullMeshes(aView); });
	prepareData.mPrepareCullLightsEvent = SC_ThreadPool::Get().SubmitTask([this, aView]() { CullLights(aView); });

	//CullMeshes(aView);
	//CullLights(aView);
	//AddRaytracingGeometry(aView);

	//for (SC_Future<bool>& task : shadowCullingTasks)
	//	task.Wait();
}

void SGfx_SceneGraph::AddModel(SC_Ref<SGfx_Model> /*aModel*/)
{
	//const SC_Array<SC_Ref<SGfx_MeshInstance>>& meshes = aModel->GetMeshes();
	//
	//SC_MutexLock lock(mMeshesMutex);
	//mMeshes.Add(meshes);
}

void SGfx_SceneGraph::RemoveModel(SC_Ref<SGfx_Model> /*aModel*/)
{
	//const SC_Array<SC_Ref<SGfx_MeshInstance>>& meshes = aModel->GetMeshes();
	//
	//SC_MutexLock lock(mMeshesMutex);
	//for (const SC_Ref<SGfx_MeshInstance>& mesh : meshes)
	//	mMeshes.RemoveCyclic(mesh);
}

void SGfx_SceneGraph::AddMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance)
{
	SGfx_SceneGraphAction action;
	action.mActionType = SGfx_SceneGraphActionType::Add;
	action.mObjectType = SGfx_SceneGraphObjectType::Mesh;
	action.mMeshInstance = aMeshInstance;

	SC_MutexLock lock(mPendingActionsMutex);
	mPendingActions.Add(action);
}

void SGfx_SceneGraph::RemoveMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance)
{
	SGfx_SceneGraphAction action;
	action.mActionType = SGfx_SceneGraphActionType::Remove;
	action.mObjectType = SGfx_SceneGraphObjectType::Mesh;
	action.mMeshInstance = aMeshInstance;

	SC_MutexLock lock(mPendingActionsMutex);
	mPendingActions.Add(action);
}

void SGfx_SceneGraph::SetActive(SC_Ref<SGfx_MeshInstance> aMeshInstance, bool aValue)
{
	SGfx_SceneGraphAction action;
	action.mActionType = (aValue) ? SGfx_SceneGraphActionType::Activate : SGfx_SceneGraphActionType::Inactivate;
	action.mObjectType = SGfx_SceneGraphObjectType::Mesh;
	action.mMeshInstance = aMeshInstance;

	SC_MutexLock lock(mPendingActionsMutex);
	mPendingActions.Add(action);
}

void SGfx_SceneGraph::AddLight(SC_Ref<SGfx_Light> aLight)
{
	SGfx_SceneGraphAction action;
	action.mActionType = SGfx_SceneGraphActionType::Add;
	action.mObjectType = SGfx_SceneGraphObjectType::Light;
	action.mLight = aLight;

	SC_MutexLock lock(mPendingActionsMutex);
	mPendingActions.Add(action);
}

void SGfx_SceneGraph::RemoveLight(SC_Ref<SGfx_Light> aLight)
{
	SGfx_SceneGraphAction action;
	action.mActionType = SGfx_SceneGraphActionType::Remove;
	action.mObjectType = SGfx_SceneGraphObjectType::Light;
	action.mLight = aLight;

	SC_MutexLock lock(mPendingActionsMutex);
	mPendingActions.Add(action);
}

void SGfx_SceneGraph::SetActive(SC_Ref<SGfx_Light> aLight, bool aValue)
{
	SGfx_SceneGraphAction action;
	action.mActionType = (aValue) ? SGfx_SceneGraphActionType::Activate : SGfx_SceneGraphActionType::Inactivate;
	action.mObjectType = SGfx_SceneGraphObjectType::Light;
	action.mLight = aLight;

	SC_MutexLock lock(mPendingActionsMutex);
	mPendingActions.Add(action);
}

void SGfx_SceneGraph::AddParticleEffect(SC_Ref<SGfx_ParticleEffect> aParticleEffect)
{
	SGfx_SceneGraphAction action;
	action.mActionType = SGfx_SceneGraphActionType::Add;
	action.mObjectType = SGfx_SceneGraphObjectType::ParticleEffect;
	action.mParticleEffect = aParticleEffect;

	SC_MutexLock lock(mPendingActionsMutex);
	mPendingActions.Add(action);
}

void SGfx_SceneGraph::RemoveParticleEffect(SC_Ref<SGfx_ParticleEffect> aParticleEffect)
{
	SGfx_SceneGraphAction action;
	action.mActionType = SGfx_SceneGraphActionType::Remove;
	action.mObjectType = SGfx_SceneGraphObjectType::ParticleEffect;
	action.mParticleEffect = aParticleEffect;

	SC_MutexLock lock(mPendingActionsMutex);
	mPendingActions.Add(action);
}

void SGfx_SceneGraph::SetActive(SC_Ref<SGfx_ParticleEffect> aParticleEffect, bool aValue)
{
	SGfx_SceneGraphAction action;
	action.mActionType = (aValue) ? SGfx_SceneGraphActionType::Activate : SGfx_SceneGraphActionType::Inactivate;
	action.mObjectType = SGfx_SceneGraphObjectType::ParticleEffect;
	action.mParticleEffect = aParticleEffect;

	SC_MutexLock lock(mPendingActionsMutex);
	mPendingActions.Add(action);
}

void SGfx_SceneGraph::AddDecal(SC_Ref<SGfx_Decal> aDecal)
{
	SGfx_SceneGraphAction action;
	action.mActionType = SGfx_SceneGraphActionType::Add;
	action.mObjectType = SGfx_SceneGraphObjectType::Decal;
	action.mDecal = aDecal;

	SC_MutexLock lock(mPendingActionsMutex);
	mPendingActions.Add(action);
}

void SGfx_SceneGraph::RemoveDecal(SC_Ref<SGfx_Decal> aDecal)
{
	SGfx_SceneGraphAction action;
	action.mActionType = SGfx_SceneGraphActionType::Remove;
	action.mObjectType = SGfx_SceneGraphObjectType::Decal;
	action.mDecal = aDecal;

	SC_MutexLock lock(mPendingActionsMutex);
	mPendingActions.Add(action);
}

void SGfx_SceneGraph::SetActive(SC_Ref<SGfx_Decal> aDecal, bool aValue)
{
	SGfx_SceneGraphAction action;
	action.mActionType = (aValue) ? SGfx_SceneGraphActionType::Activate : SGfx_SceneGraphActionType::Inactivate;
	action.mObjectType = SGfx_SceneGraphObjectType::Decal;
	action.mDecal = aDecal;

	SC_MutexLock lock(mPendingActionsMutex);
	mPendingActions.Add(action);
}

void SGfx_SceneGraph::FlushPendingActions()
{
	SC_MutexLock lock(mPendingActionsMutex);
	for (SGfx_SceneGraphAction& action : mPendingActions)
	{
		switch (action.mActionType)
		{
		case SGfx_SceneGraphActionType::Add:
			if (action.mObjectType == SGfx_SceneGraphObjectType::Mesh)
				Add_Internal(action.mMeshInstance);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::Light)
				Add_Internal(action.mLight);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::ParticleEffect)
				Add_Internal(action.mParticleEffect);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::Decal)
				Add_Internal(action.mDecal);
			break;
		case SGfx_SceneGraphActionType::Remove:
			if (action.mObjectType == SGfx_SceneGraphObjectType::Mesh)
				Remove_Internal(action.mMeshInstance);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::Light)
				Remove_Internal(action.mLight);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::ParticleEffect)
				Remove_Internal(action.mParticleEffect);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::Decal)
				Remove_Internal(action.mDecal);
			break;
		case SGfx_SceneGraphActionType::Move:
			if (action.mObjectType == SGfx_SceneGraphObjectType::Mesh)
				Move_Internal(action.mMeshInstance);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::Light)
				Move_Internal(action.mLight);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::ParticleEffect)
				Move_Internal(action.mParticleEffect);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::Decal)
				Move_Internal(action.mDecal);
			break;
		case SGfx_SceneGraphActionType::Activate:
			if (action.mObjectType == SGfx_SceneGraphObjectType::Mesh)
				SetActive_Internal(action.mMeshInstance, true);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::Light)
				SetActive_Internal(action.mLight, true);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::ParticleEffect)
				SetActive_Internal(action.mParticleEffect, true);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::Decal)
				SetActive_Internal(action.mDecal, true);
			break;
		case SGfx_SceneGraphActionType::Inactivate:
			if (action.mObjectType == SGfx_SceneGraphObjectType::Mesh)
				SetActive_Internal(action.mMeshInstance, false);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::Light)
				SetActive_Internal(action.mLight, false);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::ParticleEffect)
				SetActive_Internal(action.mParticleEffect, false);
			else if (action.mObjectType == SGfx_SceneGraphObjectType::Decal)
				SetActive_Internal(action.mDecal, false);
			break;
		}
	}

	mPendingActions.RemoveAll();
}

void SGfx_SceneGraph::Add_Internal(SC_Ref<SGfx_MeshInstance> aMeshInstance)
{
	mMeshes.Add(aMeshInstance);
}

void SGfx_SceneGraph::Add_Internal(SC_Ref<SGfx_Light> aLight)
{
	mLights.Add(aLight);
}

void SGfx_SceneGraph::Add_Internal(SC_Ref<SGfx_ParticleEffect> aParticleEffect)
{
	mParticleEffects.Add(aParticleEffect);
}

void SGfx_SceneGraph::Add_Internal(SC_Ref<SGfx_Decal> aDecal)
{
	mDecals.Add(aDecal);
}

void SGfx_SceneGraph::Remove_Internal(SC_Ref<SGfx_MeshInstance> aMeshInstance)
{
	mMeshes.RemoveCyclic(aMeshInstance);
}

void SGfx_SceneGraph::Remove_Internal(SC_Ref<SGfx_Light> aLight)
{
	mLights.RemoveCyclic(aLight);
}

void SGfx_SceneGraph::Remove_Internal(SC_Ref<SGfx_ParticleEffect> aParticleEffect)
{
	mParticleEffects.RemoveCyclic(aParticleEffect);
}

void SGfx_SceneGraph::Remove_Internal(SC_Ref<SGfx_Decal> aDecal)
{
	mDecals.RemoveCyclic(aDecal);
}

void SGfx_SceneGraph::Move_Internal(SC_Ref<SGfx_MeshInstance> /*aMeshInstance*/)
{

}

void SGfx_SceneGraph::Move_Internal(SC_Ref<SGfx_Light> /*aLight*/)
{

}

void SGfx_SceneGraph::Move_Internal(SC_Ref<SGfx_ParticleEffect> /*aParticleEffect*/)
{

}

void SGfx_SceneGraph::Move_Internal(SC_Ref<SGfx_Decal> /*aDecal*/)
{

}

void SGfx_SceneGraph::SetActive_Internal(SC_Ref<SGfx_MeshInstance> /*aMeshInstance*/, bool /*aValue*/)
{

}

void SGfx_SceneGraph::SetActive_Internal(SC_Ref<SGfx_Light> /*aLight*/, bool /*aValue*/)
{

}

void SGfx_SceneGraph::SetActive_Internal(SC_Ref<SGfx_ParticleEffect> /*aParticleEffect*/, bool /*aValue*/)
{

}

void SGfx_SceneGraph::SetActive_Internal(SC_Ref<SGfx_Decal> /*aDecal*/, bool /*aValue*/)
{

}

void SGfx_SceneGraph::CullMeshes(SGfx_View* aView)
{
	SC_PROFILER_FUNCTION();

	SGfx_ViewData& prepareData = aView->GetPrepareData();
	const SGfx_Camera& camera = aView->GetCamera();
	const SGfx_Frustum& frustum = camera.GetFrustum();

	const bool depthOnly = aView->IsOnlyDepth();

	for (const SC_Ref<SGfx_MeshInstance>& mesh : mMeshes)
	{
		if (!mesh)
			continue;

		if (!mesh->GetMaterialInstance())
			continue;

		const SC_AABB& boundingBox = mesh->GetBoundingBox();
		const SC_Sphere boundingSphere(boundingBox);
		const float distanceToCamera = (camera.GetPosition() - boundingBox.GetCenter()).Length();

#if SR_ENABLE_RAYTRACING
		if (!depthOnly && mesh->IncludeInRaytracingScene()) // TODO: FIX THIS CULLING
		{
			SR_RaytracingInstanceProperties& rtInstanceData = prepareData.mRaytracingInstances.Add(mesh->GetRaytracingInstanceProperties());
			const SR_RaytracingInstanceData instanceData = mesh->GetRaytracingInstanceData();

			uint32 rtInstanceDataOffset = 0;
			prepareData.mRaytracingInstanceData->Add(rtInstanceDataOffset, sizeof(instanceData) / sizeof(SC_Vector4), reinterpret_cast<const SC_Vector4*>(&instanceData));
			rtInstanceData.mInstanceId = rtInstanceDataOffset;
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
			SGfx_RenderQueueItem renderQueueItem;

#if SR_ENABLE_MESH_SHADERS
			if (SR_RenderDevice::gInstance->GetSupportCaps().mEnableMeshShaders && meshTemplate->IsUsingMeshlets())
			{
				const SGfx_MeshletBuffers& meshletBuffers = meshTemplate->GetMeshletBuffers();
				renderQueueItem.mMeshletData.mVertexBuffer = meshletBuffers.mVertexBuffer;
				renderQueueItem.mMeshletData.mMeshletBuffer = meshletBuffers.mMeshletBuffer;
				renderQueueItem.mMeshletData.mVertexIndexBuffer = meshletBuffers.mVertexIndexBuffer;
				renderQueueItem.mMeshletData.mPrimitiveIndexBuffer = meshletBuffers.mPrimitiveIndexBuffer;
				renderQueueItem.mUsingMeshlets = true;
			}
			else
#endif
			{
				renderQueueItem.mVertexBuffer = meshTemplate->GetVertexBufferResource();
				renderQueueItem.mIndexBuffer = meshTemplate->GetIndexBufferResource();
				renderQueueItem.mUsingMeshlets = false;
			}

			renderQueueItem.mTransform = mesh->GetTransform();
			renderQueueItem.mPrevTransform = mesh->GetPrevTransform();

			renderQueueItem.mMaterialIndex = mesh->GetMaterialInstance()->GetMaterialIndex();

			renderQueueItem.mSortDistance = distanceToCamera;

			SGfx_MaterialShaderType depthType = SGfx_MaterialShaderType::Depth;
			SGfx_MaterialShaderType shadowDepthType = SGfx_MaterialShaderType::ShadowDepth;
			SGfx_MaterialShaderType colorType = SGfx_MaterialShaderType::Default;
			if (meshTemplate->IsUsingMeshlets())
			{
				depthType = SGfx_MaterialShaderType::DepthMeshlet;
				shadowDepthType = SGfx_MaterialShaderType::ShadowDepthMeshlet;
				colorType = SGfx_MaterialShaderType::DefaultMeshlet;
			}

			SR_ShaderState* depthShader = mesh->GetMaterialInstance()->GetMaterialTemplate()->GetShaderState(meshTemplate->GetVertexLayout(), (depthOnly) ? shadowDepthType : depthType);
			SR_ShaderState* colorShader = (depthOnly) ? depthShader : mesh->GetMaterialInstance()->GetMaterialTemplate()->GetShaderState(meshTemplate->GetVertexLayout(), colorType);
			if (!depthShader || !colorShader)
				continue;

			renderQueueItem.mShader = depthShader;

			bool outputVelocity = mesh->GetMaterialInstance()->GetMaterialTemplate()->OutputVelocity();
			if (outputVelocity)
				prepareData.mDepthQueue_MotionVectors.AddItem(renderQueueItem);
			else
				prepareData.mDepthQueue.AddItem(renderQueueItem);

			if (!depthOnly)
			{
				renderQueueItem.mShader = colorShader;
				prepareData.mOpaqueQueue.AddItem(renderQueueItem);
			}
		}
	}
	prepareData.mDepthQueue.Prepare(prepareData);
	prepareData.mDepthQueue_MotionVectors.Prepare(prepareData); 
	prepareData.mOpaqueQueue.Prepare(prepareData);
	prepareData.mInstanceData->Prepare();
#if SR_ENABLE_RAYTRACING
	prepareData.mRaytracingInstanceData->Prepare();
#endif
}

void SGfx_SceneGraph::CullLights(SGfx_View* aView)
{
	SC_PROFILER_FUNCTION();

	SGfx_ViewData& prepareData = aView->GetPrepareData();
	const SGfx_Camera& camera = aView->GetCamera();
	const SGfx_Frustum& frustum = camera.GetFrustum();

	for (const SC_Ref<SGfx_Light>& light : mLights)
	{
		if (light->GetType() == SGfx_LightType::Directional || frustum.Intersects(light->GetBoundingSphere()))
		{
			// Add to light queues because this light is visible inside our current view
			SGfx_LightRenderData lightRenderData;
			lightRenderData.mCastShadow = false;
			lightRenderData.mGPUData = light->GetShaderData();
			prepareData.mVisibleLights.Add(lightRenderData);
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
