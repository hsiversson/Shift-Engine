#include "SGF_Entity.h"
#include "SGF_EntityGraph.h"
#include "GameFramework/GameWorld/SGF_World.h"
#include "GameFramework/Entity/Components/SGF_Component.h"

SGF_Entity::SGF_Entity()
	: mWorld(nullptr)
{

}

SGF_Entity::~SGF_Entity()
{

}

void SGF_Entity::Update()
{
	for (auto& comp : mComponents)
	{
		comp.second->OnUpdate();
	}
}

void SGF_Entity::SetWorld(SGF_World* aWorld)
{
	mWorld = aWorld;
}

SGF_World* SGF_Entity::GetWorld() const
{
	return mWorld;
}

bool SGF_Entity::HasComponent(const SGF_ComponentId& aComponentId)
{
	return mComponents.find(aComponentId) != mComponents.end();
}

SGF_Component* SGF_Entity::GetComponent(const SGF_ComponentId& aComponentId)
{
	if (HasComponent(aComponentId))
		return mComponents.at(aComponentId).get();

	return nullptr;
}

void SGF_Entity::AddComponent(SC_Ref<SGF_Component> aComponent)
{
	mComponents[aComponent->GetId()] = aComponent;
	aComponent->SetParentEntity(this);
}

void SGF_Entity::SetName(const std::string& aName)
{
	mName = aName;
}

const std::string& SGF_Entity::GetName() const
{
	return mName;
}

bool SGF_Entity::Save(SC_Json& aOutSaveData) const
{
	aOutSaveData["Name"] = mName;
	aOutSaveData["Tag"] = std::string("None");

	for (const auto& pair : mComponents)
	{
		const SC_Ref<SGF_Component>& component = pair.second;
		SC_Json componentSaveData;
		component->Save(componentSaveData);
		aOutSaveData["Components"].push_back(componentSaveData);
	}
	return true;
}

bool SGF_Entity::Load(const SC_Json& aSavedData)
{
	mName = aSavedData["Name"].get<std::string>();
	//mTag = aSavedData["Tag"].get<std::string>();
	
	for (const SC_Json& componentData : aSavedData["Components"])
	{
		SC_Ref<SGF_Component> component = SGF_Component::CreateFromName(componentData["Name"].get<std::string>().c_str());
		component->SetParentEntity(this);
		if (!component->Load(componentData))
			continue;
	
		AddComponent(component);
	}

	return true;
}
