#include "SGF_Entity.h"
#include "SGF_EntityGraph.h"
#include "GameFramework/GameWorld/SGF_World.h"
#include "Components/SGF_Component.h"
#include "Components/SGF_EntityIdComponent.h"

SGF_Entity::SGF_Entity()
	: mHandle(SGF_InvalidEntityHandle)
	, mEntityManager(nullptr)
	, mComponentManager(nullptr)
	, mParentWorld(nullptr)
{
}

SGF_Entity::SGF_Entity(const SGF_Entity& aOther)
	: mHandle(aOther.mHandle)
	, mEntityManager(aOther.mEntityManager)
	, mComponentManager(aOther.mComponentManager)
	, mParentWorld(aOther.mParentWorld)
{
}

SGF_Entity::SGF_Entity(SGF_Entity&& aOther)
	: mHandle(aOther.mHandle)
	, mEntityManager(aOther.mEntityManager)
	, mComponentManager(aOther.mComponentManager)
	, mParentWorld(aOther.mParentWorld)
{
	aOther.mHandle = SGF_InvalidEntityHandle;
	aOther.mEntityManager = nullptr;
	aOther.mComponentManager = nullptr;
	aOther.mParentWorld = nullptr;
}

SGF_Entity::~SGF_Entity()
{
}

SGF_Entity::operator bool() const
{
	return mHandle != SGF_InvalidEntityHandle;
}

SGF_Entity& SGF_Entity::operator=(const SGF_Entity& aOther)
{
	mHandle = aOther.mHandle;
	mEntityManager = aOther.mEntityManager;
	mComponentManager = aOther.mComponentManager;
	mParentWorld = aOther.mParentWorld;

	return *this;
}

SGF_Entity& SGF_Entity::operator=(SGF_Entity&& aOther)
{
	mHandle = aOther.mHandle;
	mEntityManager = aOther.mEntityManager;
	mComponentManager = aOther.mComponentManager;
	mParentWorld = aOther.mParentWorld;

	aOther.mHandle = SGF_InvalidEntityHandle;
	aOther.mEntityManager = nullptr;
	aOther.mComponentManager = nullptr;
	aOther.mParentWorld = nullptr;

	return *this;
}

bool SGF_Entity::operator!=(const SGF_Entity& aOther) const
{
	return mHandle != aOther.mHandle;
}

bool SGF_Entity::operator==(const SGF_Entity& aOther) const
{
	return mHandle == aOther.mHandle;
}

bool SGF_Entity::operator!=(const SGF_EntityHandle& aOther) const
{
	return mHandle != aOther;
}

bool SGF_Entity::operator==(const SGF_EntityHandle& aOther) const
{
	return mHandle == aOther;
}

bool SGF_Entity::HasComponent(const SGF_ComponentId& aId) const
{
	if (SGF_ComponentListBase* list = mComponentManager->GetList(aId))
		return list->Has(mHandle);

	return false;
}

SGF_Component* SGF_Entity::GetComponent(const SGF_ComponentId& aId) const
{
	if (SGF_ComponentListBase* list = mComponentManager->GetList(aId))
		return list->GetBase(mHandle);

	return nullptr;
}

SGF_Component* SGF_Entity::AddComponent(const SGF_ComponentId& aId) const
{
	SGF_Component* component = nullptr;
	if (SGF_ComponentListBase* list = mComponentManager->GetList(aId))
	{
		if (SGF_Component* comp = list->GetBase(mHandle))
			component = comp;
		else
			component = list->AddBase(mHandle);
	}

	if (component)
		component->SetParentEntity(*this);

	return component;
}

SGF_Component* SGF_Entity::AddComponent(const char* aComponentName) const
{
	SGF_Component* component = nullptr;
	if (SGF_ComponentListBase* list = mComponentManager->GetList(aComponentName))
	{
		if (SGF_Component* comp = list->GetBase(mHandle))
			component = comp;
		else
			component = list->AddBase(mHandle);
	}

	if (component)
		component->SetParentEntity(*this);

	return component;
}

void SGF_Entity::RemoveComponent(const SGF_ComponentId& aId) const
{
	if (SGF_ComponentListBase* list = mComponentManager->GetList(aId))
	{
		if (list->Has(mHandle))
			list->Remove(mHandle);
	}
}

const SGF_EntityHandle& SGF_Entity::GetHandle() const
{
	return mHandle;
}

SGF_EntityManager* SGF_Entity::GetEntityManager() const
{
	return mEntityManager;
}

SGF_ComponentManager* SGF_Entity::GetComponentManager() const
{
	return mComponentManager;
}

SGF_World* SGF_Entity::GetWorld() const
{
	return mParentWorld;
}

SGF_Entity::SGF_Entity(const SGF_EntityHandle& aHandle, SGF_EntityManager* aEntityManager, SGF_ComponentManager* aComponentManager, SGF_World* aParentWorld)
	: mHandle(aHandle)
	, mEntityManager(aEntityManager)
	, mComponentManager(aComponentManager)
	, mParentWorld(aParentWorld)
{
}

SGF_EntityManager::SGF_EntityManager(SGF_World* aWorld, SGF_ComponentManager* aComponentManager)
	: mLatestAllocatedEntityHandle(0)
	, mWorld(aWorld)
	, mComponentManager(aComponentManager)
{

}

SGF_EntityManager::~SGF_EntityManager()
{
}

bool SGF_EntityManager::Init()
{
	GrowEntityAllocation();
	return true;
}

SGF_Entity SGF_EntityManager::CreateEntity()
{
	SC_MutexLock lock(mMutex);
	if (mAvailableEntityHandles.IsEmpty())
		GrowEntityAllocation();

	SGF_EntityHandle newHandle = mAvailableEntityHandles.Last();
	mAvailableEntityHandles.RemoveLast();
	mAliveEntityHandles.Add(newHandle);

	SGF_Entity newEntity(newHandle, this, mComponentManager, mWorld);
	return newEntity;
}

void SGF_EntityManager::DestroyEntity(const SGF_Entity& aEntity)
{
	mComponentManager->DestroyAllComponentsForEntity(aEntity);
	SC_MutexLock lock(mMutex);
	mAliveEntityHandles.RemoveCyclic(aEntity);
	mAvailableEntityHandles.Add(aEntity);
}

void SGF_EntityManager::GetEntities(SC_Array<SGF_Entity>& aOutEntities)
{
	aOutEntities.Reserve(mAliveEntityHandles.Count());
	for (const SGF_EntityHandle& handle : mAliveEntityHandles)
	{
		aOutEntities.Add(SGF_Entity(handle, this, mComponentManager, mWorld));
	}
}

void SGF_EntityManager::GrowEntityAllocation()
{
	uint32 currentAllocSize = mAvailableEntityHandles.Capacity();
	uint32 sizeToAdd = (currentAllocSize == 0) ? 1024 : static_cast<uint32>(currentAllocSize * 0.5f);

	mAvailableEntityHandles.Reserve(currentAllocSize + sizeToAdd);
	for (uint32 i = 0; i < sizeToAdd; ++i)
	{
		mAvailableEntityHandles.Add(mLatestAllocatedEntityHandle);
		++mLatestAllocatedEntityHandle;
	}
}
