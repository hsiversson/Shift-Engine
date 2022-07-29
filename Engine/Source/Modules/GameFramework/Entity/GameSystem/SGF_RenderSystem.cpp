#include "SGF_RenderSystem.h"

#include "GameFramework/Entity/SGF_Entity.h"
#include "GameFramework/Entity/Components/SGF_Component.h"
#include "GameFramework/Entity/Components/SGF_StaticMeshComponent.h"
#include "GameFramework/Entity/Components/SGF_TransformComponent.h"

#include "Graphics/Mesh/SGfx_MeshInstance.h"
#include "Graphics/World/SGfx_World.h"

SGF_RenderSystem::SGF_RenderSystem()
{

}

SGF_RenderSystem::~SGF_RenderSystem()
{

}

bool SGF_RenderSystem::Init()
{
	mGraphicsWorld = SC_MakeUnique<SGfx_World>();
	return true;
}

void SGF_RenderSystem::Update()
{
	SC_PROFILER_FUNCTION();
	UpdateStaticMeshes();
	mGraphicsWorld->UpdateSceneGraph();
}

SGfx_World* SGF_RenderSystem::GetGraphicsWorld() const
{
	return mGraphicsWorld.get();
}

void SGF_RenderSystem::UpdateStaticMeshes()
{
	SGF_ComponentList<SGF_TransformComponent>* transforms = mComponentManager->GetList<SGF_TransformComponent>();
	SGF_ComponentList<SGF_StaticMeshComponent>* staticMeshComponents = mComponentManager->GetList<SGF_StaticMeshComponent>();
	if (staticMeshComponents && transforms)
	{
		SC_Array<SGF_StaticMeshComponent>& meshes = staticMeshComponents->GetList();
		for (SGF_StaticMeshComponent& staticMeshComp : meshes)
		{
			if (SGfx_MeshInstance* meshInstance = staticMeshComp.GetMeshInstance())
			{
				const SGF_Entity& entity = staticMeshComp.GetParentEntity();
				SGF_TransformComponent* transformComp = entity.GetComponent<SGF_TransformComponent>();

				if (transformComp)
					meshInstance->SetTransform(transformComp->GetTransform());
			}
		}
	}
}

void SGF_RenderSystem::UpdateLights()
{
}
