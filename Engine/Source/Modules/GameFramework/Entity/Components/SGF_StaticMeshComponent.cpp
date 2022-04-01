#include "GameFramework/Entity/Components/SGF_StaticMeshComponent.h"
#include "GameFramework/Entity/Components/SGF_TransformComponent.h"
#include "GameFramework/Entity/SGF_Entity.h"
#include "GameFramework/GameWorld/SGF_World.h"
#include "Graphics/World/SGfx_World.h"
#include "Graphics/Mesh/SGfx_MeshInstance.h"
#include "Graphics/Mesh/SGfx_MeshLoader.h"

SGF_StaticMeshComponent::SGF_StaticMeshComponent()
	: mIsVisible(true)
{
}

SGF_StaticMeshComponent::~SGF_StaticMeshComponent()
{

}

void SGF_StaticMeshComponent::OnCreate()
{
}

void SGF_StaticMeshComponent::OnUpdate()
{
	if (mMeshInstance)
	{
		SGF_TransformComponent* transformComp = GetParentEntity()->GetComponent<SGF_TransformComponent>();
		if (transformComp)
		{
			mMeshInstance->SetTransform(transformComp->GetTransform());
		}
	}
}

void SGF_StaticMeshComponent::OnDestroy()
{
	SetVisible(false);
}

void SGF_StaticMeshComponent::OnSetVisible(bool aValue)
{
	SetVisible(aValue);
}

void SGF_StaticMeshComponent::SetMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance)
{
	bool wasVisible = mIsVisible;
	SetVisible(false);

	mMeshInstance = aMeshInstance;

	if (mMaterialInstance)
		mMeshInstance->SetMaterialInstance(mMaterialInstance);

	SetVisible(wasVisible);
}

void SGF_StaticMeshComponent::SetMaterial(SC_Ref<SGfx_MaterialInstance> aMaterialInstance)
{
	if (!aMaterialInstance)
		return;
	if (mMaterialInstance == aMaterialInstance)
		return;

	mMaterialInstance = aMaterialInstance;
	if (mMeshInstance)
		mMeshInstance->SetMaterialInstance(mMaterialInstance);
}

void SGF_StaticMeshComponent::SetVisible(bool aValue)
{
	if (mIsVisible == aValue)
		return;

	mIsVisible = aValue;

	if (SGF_Entity* entity = GetParentEntity())
	{
		if (SGF_World* world = entity->GetWorld())
		{
			SGfx_World* graphicsWorld = world->GetGraphicsWorld();
			if (graphicsWorld && mMeshInstance)
			{
				if (mIsVisible)
				{
					SGF_TransformComponent* transformComp = entity->GetComponent<SGF_TransformComponent>();
					if (transformComp)
					{
						SC_Matrix transform = SC_ScaleMatrix(transformComp->mScale);
						transform *= transformComp->mRotation.AsMatrix();
						transform *= SC_TranslationMatrix(transformComp->mPosition);

						mMeshInstance->SetTransform(transform);
					}


					graphicsWorld->AddMesh(mMeshInstance);
				}
				else
					graphicsWorld->RemoveMesh(mMeshInstance);
			}
		}
	}
}

SC_Ref<SGfx_MeshInstance>& SGF_StaticMeshComponent::GetMeshInstance()
{
	return mMeshInstance;
}

SC_Ref<SGfx_MaterialInstance>& SGF_StaticMeshComponent::GetMaterialInstance()
{
	return mMaterialInstance;
}

bool SGF_StaticMeshComponent::Save(SC_Json& aOutSaveData) const
{
	if (!SGF_Component::Save(aOutSaveData))
		return false;

	aOutSaveData["Mesh"] = mMeshInstance->GetMeshTemplate()->GetSourceFile().GetStr();

	//if (mMaterialInstance->GetMaterialTemplate()->GetSourceFile().IsEmpty())
	//{
	//	mMaterialInstance->GetMaterialTemplate()->SaveToFile(mMeshInstance->GetMeshTemplate()->GetSourceFile() + ".smat");
	//}
	//
	//aOutSaveData["Material"] = mMaterialInstance->GetMaterialTemplate()->GetSourceFile().GetStr();

	return true;
}

bool SGF_StaticMeshComponent::Load(const SC_Json& aSavedData)
{
	if (!SGF_Component::Load(aSavedData))
		return false;

	SC_FilePath meshPath = SC_EnginePaths::Get().GetGameDataDirectory() + "/" + aSavedData["Mesh"].get<std::string>().c_str();

	SGfx_MeshCreateParams meshCreateParams;
	if (!SGfx_MeshLoader::Load(meshPath, meshCreateParams))
		return false;

	SetMesh(SGfx_MeshInstance::Create(meshCreateParams));
	if (!mMeshInstance)
		return false;

	// TEMP
	mMeshInstance->SetMaterialInstance(SGfx_MaterialInstance::GetDefault());

	return true;
}
