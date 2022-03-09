#include "SGF_Entity.h"
#include "SGF_EntityGraph.h"
#include "GameFramework/GameWorld/SGF_World.h"
#include "Components/SGF_Component.h"
#include "Components/SGF_EntityIdComponent.h"

SGF_Entity::SGF_Entity()
	: mWorld(nullptr)
	, mParent(nullptr)
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

bool SGF_Entity::Is(const SC_UUID& aId) const
{
	return GetComponent<SGF_EntityIdComponent>()->GetUUID() == aId;
}

const SC_Array<SGF_Entity*>& SGF_Entity::GetChildren() const
{
	return mChildren;
}

void SGF_Entity::SetParent(SGF_Entity* aParent)
{
	if (mParent)
		mParent->RemoveChild(this);

	mParent = aParent;

	if (mParent)
		mParent->AddChild(this);
}

SGF_Entity* SGF_Entity::GetParent() const
{
	return mParent;
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

void SGF_Entity::AddChild(SGF_Entity* aEntity)
{
	if (aEntity == this)
		return;
	if (aEntity->GetParent() == this)
		return;
	if (GetParent() == aEntity)
		return;

	mChildren.Add(aEntity);
	aEntity->SetParent(this);
}

void SGF_Entity::RemoveChild(SGF_Entity* aEntity)
{
	mChildren.Remove(aEntity);
}