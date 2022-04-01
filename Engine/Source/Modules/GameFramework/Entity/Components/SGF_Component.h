#pragma once
#include "GameFramework/Misc/SGF_Property.h"
#include "Graphics/Material/SGfx_MaterialInstance.h"
#include "Graphics/Mesh/SGfx_MeshInstance.h"
#include "RenderCore/Interface/SR_Texture.h"

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

#if IS_EDITOR_BUILD
class SGF_PropertyHelperBase
{
public:
	enum class Type
	{
		Unknown,
		Bool,
		Int,
		Uint,
		Float,
		Quaternion,
		Text,
		Vector,
		Color,
		Texture,
		Mesh,
		Material,
		EntityRef
	};

	SGF_PropertyHelperBase(const Type& aType, const char* aName, const char* aMetaData = nullptr) 
		: mPropertyType(aType)
		, mName(aName) 
	{
		ParseMetaData(aMetaData);
	}

	virtual ~SGF_PropertyHelperBase() {}

	template<class T>
	static Type GetTypeFromClassType()
	{
		return Type::Unknown;
	}

	const Type& GetType() const { return mPropertyType; }
	const char* GetName() const { return mName; }

	std::string GetMetaDataFromKey(const char* aKey) const
	{
		if (mMetaData.find(aKey) != mMetaData.end())
			return mMetaData.at(aKey);

		return std::string();
	}

private:
	void ParseMetaData(const char* aMetaData)
	{
		if (!aMetaData)
			return;

		static constexpr char metaDataDelimiter = ',';
		static constexpr char keyValueDelimiter = '=';

		std::string metaDataStr(aMetaData);
		size_t lastCommaPos = 0;
		size_t nextCommaPos = metaDataStr.find(metaDataDelimiter, 0);
		bool shouldBreakOnNextIter = false;
		while (true)
		{
			size_t len = nextCommaPos - lastCommaPos;
			std::string keyValue = metaDataStr.substr(lastCommaPos, len);
			size_t delimPos = keyValue.find(keyValueDelimiter);
			std::string key = keyValue.substr(0, delimPos);
			std::string value = keyValue.substr(delimPos + 1, nextCommaPos);

			mMetaData[key] = value;

			if (shouldBreakOnNextIter)
				break;

			lastCommaPos = nextCommaPos + 1; // Skip one for the comma itself
			nextCommaPos = metaDataStr.find(metaDataDelimiter, lastCommaPos);
			shouldBreakOnNextIter = nextCommaPos == std::string::npos;
			if (shouldBreakOnNextIter)
			{
				nextCommaPos = metaDataStr.length();
			}
		}
	}

private:
	SC_UnorderedMap<std::string, std::string> mMetaData;
	const Type mPropertyType;
	const char* mName;
};
template<> inline SGF_PropertyHelperBase::Type SGF_PropertyHelperBase::GetTypeFromClassType<bool>() { return Type::Bool; }
template<> inline SGF_PropertyHelperBase::Type SGF_PropertyHelperBase::GetTypeFromClassType<int32>() { return Type::Int; }
template<> inline SGF_PropertyHelperBase::Type SGF_PropertyHelperBase::GetTypeFromClassType<uint32>() { return Type::Uint; }
template<> inline SGF_PropertyHelperBase::Type SGF_PropertyHelperBase::GetTypeFromClassType<float>() { return Type::Float; }
template<> inline SGF_PropertyHelperBase::Type SGF_PropertyHelperBase::GetTypeFromClassType<const char*>() { return Type::Text; }
template<> inline SGF_PropertyHelperBase::Type SGF_PropertyHelperBase::GetTypeFromClassType<SC_Vector>() { return Type::Vector; }
template<> inline SGF_PropertyHelperBase::Type SGF_PropertyHelperBase::GetTypeFromClassType<SC_Color>() { return Type::Color; }
template<> inline SGF_PropertyHelperBase::Type SGF_PropertyHelperBase::GetTypeFromClassType<SC_Quaternion>() { return Type::Quaternion; }
template<> inline SGF_PropertyHelperBase::Type SGF_PropertyHelperBase::GetTypeFromClassType<SC_Ref<SR_Texture>>() { return Type::Texture; }
template<> inline SGF_PropertyHelperBase::Type SGF_PropertyHelperBase::GetTypeFromClassType<SC_Ref<SGfx_MeshInstance>>() { return Type::Mesh; }
template<> inline SGF_PropertyHelperBase::Type SGF_PropertyHelperBase::GetTypeFromClassType<SC_Ref<SGfx_MaterialInstance>>() { return Type::Material; }
template<> inline SGF_PropertyHelperBase::Type SGF_PropertyHelperBase::GetTypeFromClassType<SC_Ref<SGF_Entity>>() { return Type::EntityRef; }

template<class T>
class SGF_PropertyHelper : public SGF_PropertyHelperBase
{
public:
	SGF_PropertyHelper(SGF_Component* aParentComponent, const char* aName, T& aProperty)
		: SGF_PropertyHelperBase(SGF_PropertyHelperBase::GetTypeFromClassType<T>(), aName)
		, mPropertyRef(aProperty)
	{
		if (aParentComponent)
			aParentComponent->AddProperty(this);
	}

	SGF_PropertyHelper(SGF_Component* aParentComponent, const char* aName, T& aProperty, const char* aMetaData)
		: SGF_PropertyHelperBase(SGF_PropertyHelperBase::GetTypeFromClassType<T>(), aName, aMetaData)
		, mPropertyRef(aProperty)
	{
		if (aParentComponent)
			aParentComponent->AddProperty(this);
	}

	T& Get() { return mPropertyRef; }
	const T& Get() const { return mPropertyRef; }

private:
	T& mPropertyRef;
};
#endif

class SGF_Component
{
	friend class SGF_Entity;
public:
	static SC_Ref<SGF_Component> CreateFromId(const SGF_ComponentId& aComponentId);
	static SC_Ref<SGF_Component> CreateFromName(const char* aComponentName);
	static const SGF_ComponentId& GetIdFromName(const char* aComponentName);

public:
	SGF_Component();
	virtual ~SGF_Component();

	virtual void OnCreate() {}
	virtual void OnStart() {}
	virtual void OnUpdate() {}
	virtual void OnDestroy() {}

	virtual bool Save(SC_Json& aOutSaveData) const;
	virtual bool Load(const SC_Json& aSavedData);

	virtual SGF_ComponentId GetId() const;
	virtual const char* GetName() const;

	SGF_Entity* GetParentEntity() const;

#if IS_EDITOR_BUILD
	void AddProperty(SGF_PropertyHelperBase* aProperty) { mProperties.Add(aProperty); }
	const SC_Array<SGF_PropertyHelperBase*>& GetProperties() const { return mProperties; }
#endif

public:
	template<class ComponentType>
	static void RegisterComponent()
	{
		SGF_ComponentFactory::RegistryEntry<ComponentType>::Instance(ComponentType::Id(), ComponentType::Name());
	}

protected:
#if IS_EDITOR_BUILD
	SC_Array<SGF_PropertyHelperBase*> mProperties;
#endif

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


#if IS_EDITOR_BUILD
	#define SGF_PROPERTY(aVariableType, aVariableName, aPropertyName, ...)	\
			aVariableType aVariableName;		\
			SGF_PropertyHelper<aVariableType> SC_CONCAT(__nHelper, aVariableName) = SGF_PropertyHelper<aVariableType>(this, aPropertyName, aVariableName, __VA_ARGS__)
#else
	#define SGF_PROPERTY(aVariableType, aVariableName, aPropertyName, ...) aVariableType aVariableName;
#endif