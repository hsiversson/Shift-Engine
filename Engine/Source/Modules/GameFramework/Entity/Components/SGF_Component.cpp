#include "SGF_Component.h"

SC_Ref<SGF_Component> SGF_Component::CreateFromId(const SGF_ComponentId& aComponentId)
{
	SGF_ComponentFactory::Registry& registry = SGF_ComponentFactory::GetComponentRegistry();
	auto it = registry.find(aComponentId);
	if (it == registry.end())
	{
		assert(false && "No component with this id has been registered.");
		return nullptr;
	}
	return it->second();
}

SC_Ref<SGF_Component> SGF_Component::CreateFromName(const char* aComponentName)
{
	SGF_ComponentFactory::RegistryMap& registryMap = SGF_ComponentFactory::GetComponentRegistryMap();
	auto it = registryMap.find(aComponentName);
	if (it == registryMap.end())
	{
		assert(false && "No component with this name has been registered.");
		return nullptr;
	}
	SGF_ComponentFactory::Registry& registry = SGF_ComponentFactory::GetComponentRegistry();
	auto it2 = registry.find(it->second);
	return it2->second();
}

const SGF_ComponentId& SGF_Component::GetIdFromName(const char* aComponentName)
{
	SGF_ComponentFactory::RegistryMap& registryMap = SGF_ComponentFactory::GetComponentRegistryMap();
	auto it = registryMap.find(aComponentName);
	if (it == registryMap.end())
	{
		assert(false && "No component with this name has been registered.");
		return SGF_InvalidComponentId;
	}
	return registryMap.at(aComponentName);
}

SGF_Component::SGF_Component()
	: mParentEntity(nullptr)
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

SGF_Entity* SGF_Component::GetParentEntity() const
{
	return mParentEntity;
}

void SGF_Component::SetParentEntity(SGF_Entity* aParentEntity)
{
	mParentEntity = aParentEntity;
}
