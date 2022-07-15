#pragma once
#include "SGF_EntityCommon.h"
#include "GameFramework/Entity/Components/SGF_ComponentCommon.h"

class SGF_Component;
class SGF_EntityGraph;
class SGF_EntityManager;
class SGF_ComponentManager;
class SGF_World;

class SGF_Entity
{
	friend class SGF_EntityManager;
	friend class SED_WorldHierarchyWindow;
public:
	SGF_Entity();
	SGF_Entity(const SGF_Entity& aOther);
	SGF_Entity(SGF_Entity&& aOther);
	~SGF_Entity();

	operator bool() const;
	SGF_Entity& operator=(const SGF_Entity& aOther);
	SGF_Entity& operator=(SGF_Entity&& aOther);
	bool operator==(const SGF_Entity& aOther) const;
	bool operator!=(const SGF_Entity& aOther) const;
	bool operator==(const SGF_EntityHandle& aOther) const;
	bool operator!=(const SGF_EntityHandle& aOther) const;

	bool HasComponent(const SGF_ComponentId& aId) const;
	SGF_Component* GetComponent(const SGF_ComponentId& aId) const;
	SGF_Component* AddComponent(const SGF_ComponentId& aId) const;
	SGF_Component* AddComponent(const char* aComponentName) const;
	void RemoveComponent(const SGF_ComponentId& aId) const;

	template<class ComponentType>
	bool HasComponent() const
	{
		return HasComponent(ComponentType::Id());
	}

	template<class ComponentType>
	ComponentType* GetComponent() const
	{
		return static_cast<ComponentType*>(GetComponent(ComponentType::Id()));
	}

	template<class ComponentType>
	ComponentType* AddComponent() const
	{
		return static_cast<ComponentType*>(AddComponent(ComponentType::Id()));
	}

	template<class ComponentType>
	void RemoveComponent() const
	{
		RemoveComponent(ComponentType::Id());
	}

	const SGF_EntityHandle& GetHandle() const;
	SGF_EntityManager* GetEntityManager() const;
	SGF_ComponentManager* GetComponentManager() const;
	SGF_World* GetWorld() const;

private:
	SGF_Entity(const SGF_EntityHandle& aHandle, SGF_EntityManager* aEntityManager, SGF_ComponentManager* aComponentManager, SGF_World* aParentWorld);

	SGF_EntityHandle mHandle;
	SGF_EntityManager* mEntityManager;
	SGF_ComponentManager* mComponentManager;
	SGF_World* mParentWorld;
};

class SGF_EntityManager
{
public:
	SGF_EntityManager(SGF_World* aWorld, SGF_ComponentManager* aComponentManager);
	~SGF_EntityManager();

	bool Init();

	SGF_Entity CreateEntity();
	void DestroyEntity(const SGF_Entity& aEntity);

	void GetEntities(SC_Array<SGF_Entity>& aOutEntities);

private:
	void GrowEntityAllocation();

	SC_Array<SGF_EntityHandle> mAvailableEntityHandles;
	SC_Array<SGF_EntityHandle> mAliveEntityHandles;

	SC_Mutex mMutex;
	uint32 mLatestAllocatedEntityHandle;

	SGF_World* mWorld;
	SGF_ComponentManager* mComponentManager;
};
