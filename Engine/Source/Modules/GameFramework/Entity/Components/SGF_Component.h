#pragma once
#include "GameFramework/Misc/SGF_Property.h"
#include "GameFramework/Entity/Components/SGF_ComponentCommon.h"
#include "GameFramework/Entity/SGF_Entity.h"

class SGF_Entity;
class SGF_Component : public SGF_PropertyContainer
{
	friend class SGF_Entity;
public:
	SGF_Component();
	virtual ~SGF_Component();

	virtual void OnCreate() {}
	virtual void OnUpdate() {}
	virtual void OnDestroy() {}

	virtual bool Save(SC_Json& aOutSaveData) const;
	virtual bool Load(const SC_Json& aSavedData);

	virtual SGF_ComponentId GetId() const;
	virtual const char* GetName() const;

	// Misc overrides
	virtual void OnSetVisible(bool /*aValue*/) {}

	const SGF_Entity& GetParentEntity() const;

private:
	void SetParentEntity(const SGF_Entity& aParentEntity);

	SGF_Entity mParentEntity;
};
SC_ALLOW_MEMCPY_RELOCATE(SGF_Component);

class SGF_ComponentListBase
{
public:
	SGF_ComponentListBase() {}
	virtual ~SGF_ComponentListBase() {}

	virtual bool Has(const SGF_EntityHandle& /*aHandle*/) const { return false; }
	virtual SGF_Component* GetBase(const SGF_EntityHandle& /*aHandle*/) { return nullptr; }
	virtual const SGF_Component* GetBase(const SGF_EntityHandle& /*aHandle*/) const { return nullptr; }
	virtual SGF_Component* AddBase(const SGF_EntityHandle& /*aHandle*/) { return nullptr; }
	virtual void Remove(const SGF_EntityHandle& /*aHandle*/) {}
	virtual uint32 GetNumComponents() const { return 0; }

#if IS_EDITOR_BUILD
	virtual const SC_Array<SGF_PropertyHelperBase*>& GetProperties(const SGF_EntityHandle& aHandle) = 0;
#endif

protected:
};

#define COMPONENT_DECLARE(aComponentClass, aName)														\
public:																									\
	static SGF_ComponentId Id()																			\
	{																									\
		static const SGF_ComponentId id = typeid(aComponentClass).hash_code();							\
		return id;																						\
	}																									\
	virtual SGF_ComponentId GetId() const override { return Id(); }										\
	static const char* Name() { return aName; }															\
	virtual const char* GetName() const override { return Name(); }


template<class ComponentType>
class SGF_ComponentList : public SGF_ComponentListBase
{
public:
	SGF_ComponentList() {}
	~SGF_ComponentList() {}

	ComponentType* Add(const SGF_EntityHandle& aHandle)
	{
		if (Has(aHandle))
			return &mComponents[mEntityToComponentMap[aHandle]];

		SC_MutexLock lock(mMutex);
		ComponentType& newComp = mComponents.Add();
		uint32 newCompIdx = mComponents.Count() - 1;
		mEntityToComponentMap[aHandle] = newCompIdx;
		mComponentToEntityMap[newCompIdx] = aHandle;

		return &newComp;
	}

	void Remove(const SGF_EntityHandle& aHandle) override
	{
		if (Has(aHandle))
		{
			uint32 currentLastCompIdx = mComponents.Count() - 1;
			SGF_EntityHandle currentLastEntityHandle = mComponentToEntityMap[currentLastCompIdx];

			SC_MutexLock lock(mMutex);
			uint32 compIdx = mEntityToComponentMap[aHandle];
			mEntityToComponentMap.erase(aHandle);
			mComponentToEntityMap.erase(compIdx);

			if (aHandle != currentLastEntityHandle)
			{
				mComponentToEntityMap[compIdx] = currentLastEntityHandle;
				mEntityToComponentMap[currentLastEntityHandle] = compIdx;
			}

			mComponents.RemoveCyclicAt(compIdx);
		}
	}

	bool Has(const SGF_EntityHandle& aHandle) const override
	{
		return mEntityToComponentMap.contains(aHandle);
	}

	ComponentType* Get(const SGF_EntityHandle& aHandle)
	{
		if (Has(aHandle))
			return &mComponents[mEntityToComponentMap.at(aHandle)];
		else
			return nullptr;
	}
	const ComponentType* Get(const SGF_EntityHandle& aHandle) const
	{
		if (Has(aHandle))
			return &mComponents[mEntityToComponentMap.at(aHandle)];
		else
			return nullptr;
	}

	SC_Array<ComponentType>& GetList()
	{
		return mComponents;
	}

	const SC_Array<ComponentType>& GetList() const
	{
		return mComponents;
	}

	uint32 GetNumComponents() const override
	{
		return mComponents.Count();
	}

	SGF_Component* GetBase(const SGF_EntityHandle& aHandle) override
	{
		return static_cast<SGF_Component*>(Get(aHandle));
	}

	const SGF_Component* GetBase(const SGF_EntityHandle& aHandle) const override
	{
		return static_cast<const SGF_Component*>(Get(aHandle));
	}

	SGF_Component* AddBase(const SGF_EntityHandle& aHandle) override
	{
		return static_cast<SGF_Component*>(Add(aHandle));
	}

#if IS_EDITOR_BUILD
	const SC_Array<SGF_PropertyHelperBase*>& GetProperties(const SGF_EntityHandle& aHandle) override
	{
		ComponentType& comp = mComponents[mEntityToComponentMap[aHandle]];
		return comp.GetProperties();
	}
#endif
private:
	SC_Mutex mMutex;
	SC_Array<ComponentType> mComponents;
	SC_UnorderedMap<SGF_EntityHandle, uint32> mEntityToComponentMap;
	SC_UnorderedMap<uint32, SGF_EntityHandle> mComponentToEntityMap;
};

class SGF_ComponentManager
{
public:
	SGF_ComponentManager();
	~SGF_ComponentManager();

	bool HasList(const SGF_ComponentId& aId);
	bool HasList(const char* aComponentName);
	SGF_ComponentListBase* GetList(const SGF_ComponentId& aId);
	SGF_ComponentListBase* GetList(const char* aComponentName);
	void DestroyAllComponentsForEntity(const SGF_EntityHandle& aHandle);

	template<class ComponentType>
	void RegisterComponentType()
	{
		if (HasList<ComponentType>())
			return;

		SC_Ref<SGF_ComponentList<ComponentType>> newList = SC_MakeRef<SGF_ComponentList<ComponentType>>();
		mComponentLists[ComponentType::Id()] = newList;
		mComponentNameToId[ComponentType::Name()] = ComponentType::Id();
	}

	template<class ComponentType>
	void UnregisterComponentType()
	{
		if (!HasList<ComponentType>())
			return;

		mComponentLists.erase(ComponentType::Id());
		mComponentNameToId.erase(ComponentType::Name());
	}

	template<class ComponentType>
	bool HasList()
	{
		return mComponentLists.contains(ComponentType::Id());
	}

	template<class ComponentType>
	SGF_ComponentList<ComponentType>* GetList()
	{
		if (!HasList<ComponentType>())
			return nullptr;

		return static_cast<SGF_ComponentList<ComponentType>*>(mComponentLists[ComponentType::Id()].Get());
	}

private:
	SC_UnorderedMap<SGF_ComponentId, SC_Ref<SGF_ComponentListBase>> mComponentLists;
	SC_UnorderedMap<std::string, SGF_ComponentId> mComponentNameToId;
};