#include "SGF_Component.h"
#include "GameFramework/Entity/SGF_Entity.h"

SGF_Component::SGF_Component()
{

}

SGF_Component::~SGF_Component()
{

}

bool SGF_Component::Save(SC_Json& aOutSaveData) const
{
	aOutSaveData["Name"] = GetName();
	aOutSaveData["Id"] = GetId();
	return true;
}

bool SGF_Component::Load(const SC_Json& /*aSavedData*/)
{
	return true;
}

SGF_ComponentId SGF_Component::GetId() const
{
	return SGF_InvalidComponentId;
}

const char* SGF_Component::GetName() const
{
	return "Unknown Component Type";
}

const SGF_Entity& SGF_Component::GetParentEntity() const
{
	return mParentEntity;
}

void SGF_Component::SetParentEntity(const SGF_Entity& aParentEntity)
{
	mParentEntity = aParentEntity;
}

SGF_ComponentManager::SGF_ComponentManager()
{
}

SGF_ComponentManager::~SGF_ComponentManager()
{
}

bool SGF_ComponentManager::HasList(const SGF_ComponentId& aId)
{
	return mComponentLists.contains(aId);
}

bool SGF_ComponentManager::HasList(const char* aComponentName)
{
	return mComponentNameToId.contains(aComponentName);
}

SGF_ComponentListBase* SGF_ComponentManager::GetList(const SGF_ComponentId& aId)
{
	if (!HasList(aId))
		return nullptr;

	return mComponentLists[aId];
}

SGF_ComponentListBase* SGF_ComponentManager::GetList(const char* aComponentName)
{
	if (!HasList(aComponentName))
		return nullptr;

	return mComponentLists[mComponentNameToId[aComponentName]];
}

void SGF_ComponentManager::DestroyAllComponentsForEntity(const SGF_EntityHandle& aHandle)
{
	for (const auto& compList : mComponentLists)
		compList.second->Remove(aHandle);
}

