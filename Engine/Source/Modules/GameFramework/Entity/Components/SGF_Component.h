#pragma once
#include "GameFramework/Misc/SGF_Property.h"

using SGF_ComponentId = int32;
static constexpr SGF_ComponentId SGF_InvalidComponentId = -1;

class SGF_Entity;
class SGF_Component;
class SGF_ComponentFactory
{
public:
	typedef SC_Ref<SGF_Component> (*CreateComponentFunc)();
	typedef SC_UnorderedMap<SGF_ComponentId, CreateComponentFunc> Registry;
	typedef SC_UnorderedMap<std::string, SGF_ComponentId> RegistryMap;

	static inline Registry& GetComponentRegistry()
	{
		static Registry registry;
		return registry;
	}
	static inline RegistryMap& GetComponentRegistryMap()
	{
		static RegistryMap registryMap;
		return registryMap;
	}

	static inline SC_Ref<SGF_Component> CreateComponent(const SGF_ComponentId& aId)
	{
		SC_Ref<SGF_Component> comp;
		Registry& registry = GetComponentRegistry();
		if (registry.find(aId) != registry.end())
		{
			comp = registry.at(aId)();
		}
		return comp;
	}

	template<class ComponentType>
	static SC_Ref<SGF_Component> CreateComponent()
	{
		return SC_MakeRef<ComponentType>();
	}

	template<class ComponentType>
	struct RegistryEntry
	{
	public:
		static RegistryEntry<ComponentType>& Instance(const SGF_ComponentId& aId, const char* aName)
		{
			static RegistryEntry<ComponentType> instance(aId, aName);
			return instance;
		}

	private:
		RegistryEntry(const SGF_ComponentId& aId, const char* aName)
		{
			Registry& registry = GetComponentRegistry();
			RegistryMap& registryMap = GetComponentRegistryMap();
			CreateComponentFunc constructorFunc = CreateComponent<ComponentType>;

			auto result = registry.insert(Registry::value_type(aId, constructorFunc));
			auto result2 = registryMap.insert(RegistryMap::value_type(std::string(aName), aId));

			if (result.second == false)
			{
				assert(false && "This component has already been registered.");
			}
		}

		RegistryEntry(const RegistryEntry<ComponentType>&) = delete;
		RegistryEntry& operator=(const RegistryEntry<ComponentType>&) = delete;
	};
};

class SGF_Component : public SGF_PropertyContainer
{
	friend class SGF_Entity;
public:
	static SC_Ref<SGF_Component> CreateFromId(const SGF_ComponentId& aComponentId);
	static SC_Ref<SGF_Component> CreateFromName(const char* aComponentName);
	static const SGF_ComponentId& GetIdFromName(const char* aComponentName);

public:
	SGF_Component();
	virtual ~SGF_Component();

	// Common overrides
	virtual void OnCreate() {}
	virtual void OnStart() {}
	virtual void OnUpdate() {}
	virtual void OnDestroy() {}

	// Misc overrides
	virtual void OnSetVisible(bool /*aValue*/) {}

	virtual bool Save(SC_Json& aOutSaveData) const;
	virtual bool Load(const SC_Json& aSavedData);

	virtual SGF_ComponentId GetId() const;
	virtual const char* GetName() const;

	SGF_Entity* GetParentEntity() const;

public:
	template<class ComponentType>
	static void RegisterComponent()
	{
		SGF_ComponentFactory::RegistryEntry<ComponentType>::Instance(ComponentType::Id(), ComponentType::Name());
	}

private:
	void SetParentEntity(SGF_Entity* aParentEntity);

	SGF_Entity* mParentEntity;
};

#define COMPONENT_DECLARE(aComponentClass, aName)														\
public:																									\
	static SGF_ComponentId Id()																			\
	{																									\
		static const SGF_ComponentId id = (SGF_ComponentId)std::hash<std::string>{}(#aComponentClass);	\
		return id;																						\
	}																									\
	virtual SGF_ComponentId GetId() const override { return Id(); }										\
	static const char* Name() { return aName; }															\
	virtual const char* GetName() const override { return Name(); }
