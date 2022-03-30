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

class SGF_PropertyBase
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

public:
	SGF_PropertyBase(const Type& aType) 
		: mType(aType)
		, mInternalData(nullptr) 
#if IS_EDITOR_BUILD
		, mName("Unnamed Property")
		, mInternalResetData(nullptr)
#endif
	{}

	SGF_PropertyBase(const Type& aType, void* aData) 
		: mType(aType)
		, mInternalData(aData)
#if IS_EDITOR_BUILD
		, mName("Unnamed Property")
		, mInternalResetData(nullptr)
#endif
	{}

	virtual ~SGF_PropertyBase() {}

	const Type& GetType() const { return mType; }

	template<class T>
	static Type GetTypeFromClassType()
	{
		return Type::Unknown;
	}

	void* GetData() const { return mInternalData; }

	template<class T>
	static T* GetDataAs()
	{
		return static_cast<T*>(mInternalData);
	}

#if IS_EDITOR_BUILD
	void SetName(const char* aName) { mName = aName; }
	const char* GetName() const { return mName; }

	const void* GetResetData() const { return mInternalResetData; }

	template<class T>
	static const T* GetResetDataAs()
	{
		return static_cast<T*>(mInternalResetData);
	}
#endif

protected:
	void SetInternalData(void* aData) { mInternalData = aData; }

	const Type mType;
	void* mInternalData;

#if IS_EDITOR_BUILD
	void SetInternalResetData(void* aData) { mInternalResetData = aData; }
	const char* mName;
	void* mInternalResetData;
#endif
};
template<> inline SGF_PropertyBase::Type SGF_PropertyBase::GetTypeFromClassType<bool>() { return Type::Bool; }	
template<> inline SGF_PropertyBase::Type SGF_PropertyBase::GetTypeFromClassType<int32>() { return Type::Int; }
template<> inline SGF_PropertyBase::Type SGF_PropertyBase::GetTypeFromClassType<uint32>() { return Type::Uint; }
template<> inline SGF_PropertyBase::Type SGF_PropertyBase::GetTypeFromClassType<float>() { return Type::Float; }
template<> inline SGF_PropertyBase::Type SGF_PropertyBase::GetTypeFromClassType<const char*>() { return Type::Text; }
template<> inline SGF_PropertyBase::Type SGF_PropertyBase::GetTypeFromClassType<SC_Vector>() { return Type::Vector; }
template<> inline SGF_PropertyBase::Type SGF_PropertyBase::GetTypeFromClassType<SC_Color>() { return Type::Color; }
template<> inline SGF_PropertyBase::Type SGF_PropertyBase::GetTypeFromClassType<SC_Quaternion>() { return Type::Quaternion; }
template<> inline SGF_PropertyBase::Type SGF_PropertyBase::GetTypeFromClassType<SC_Ref<SR_Texture>>() { return Type::Texture; }
template<> inline SGF_PropertyBase::Type SGF_PropertyBase::GetTypeFromClassType<SC_Ref<SGfx_MeshInstance>>() { return Type::Mesh; }
template<> inline SGF_PropertyBase::Type SGF_PropertyBase::GetTypeFromClassType<SC_Ref<SGfx_MaterialInstance>>() { return Type::Material; }
template<> inline SGF_PropertyBase::Type SGF_PropertyBase::GetTypeFromClassType<SC_Ref<SGF_Entity>>() { return Type::EntityRef; }

template<class T>
class SGF_Property : public SGF_PropertyBase
{
public:
	SGF_Property() 
		: SGF_PropertyBase(SGF_PropertyBase::GetTypeFromClassType<T>(), &mInternalPropertyVariable)
		{}

	SGF_Property(const T& aProperty)
		: SGF_PropertyBase(SGF_PropertyBase::GetTypeFromClassType<T>(), &mInternalPropertyVariable)
		, mInternalPropertyVariable(aProperty)
	{}

	void operator=(const T& aProperty) { mInternalPropertyVariable = aProperty; SetInternalData(&mInternalPropertyVariable); }

	T& operator->() { return mInternalPropertyVariable; }
	const T& operator->() const { return mInternalPropertyVariable; }

	operator T() const { return mInternalPropertyVariable; }

	bool Valid() const { return mInternalPropertyVariable != static_cast<T>(0); }

	T& Get() { return mInternalPropertyVariable; }
	const T& Get() const { return mInternalPropertyVariable; }
	void Set(const T& aProperty) { mInternalPropertyVariable = aProperty; SetInternalData(&mInternalPropertyVariable); }

#if IS_EDITOR_BUILD
	void SetResetValue(const T& aResetValue) { mResetValue = aResetValue; SetInternalResetData(&mResetValue); }
	const T& GetResetValue() const { return mResetValue; }
#endif
private:
	T mInternalPropertyVariable;
#if IS_EDITOR_BUILD
	T mResetValue;
#endif
};

class SGF_Component
{
	friend class SGF_Entity;
public:
	static SC_Ref<SGF_Component> CreateFromId(const SGF_ComponentId& aComponentId);
	static SC_Ref<SGF_Component> CreateFromName(const char* aComponentName);

public:
	SGF_Component();
	virtual ~SGF_Component();

	virtual void OnCreate() {};
	virtual void OnStart() {};
	virtual void OnUpdate() {};

	virtual bool Save(SC_Json& aOutSaveData) const;
	virtual bool Load(const SC_Json& aSavedData);

	virtual SGF_ComponentId GetId() const;
	virtual const char* GetName() const;

	SGF_Entity* GetParentEntity() const;
	const SC_Array<SGF_PropertyBase*>& GetProperties() const;

	void AddProperty(SGF_PropertyBase* aProperty) { mProperties.Add(aProperty); }

public:
	template<class ComponentType>
	static void RegisterComponent()
	{
		SGF_ComponentFactory::RegistryEntry<ComponentType>::Instance(ComponentType::Id(), ComponentType::Name());
	}

protected:
	SC_Array<SGF_PropertyBase*> mProperties;

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
	template<class T>
	struct SGF_PropertyDeclHelper
	{
		SGF_PropertyDeclHelper(SGF_Property<T>* aProperty, SGF_Component* aParentComponent, const char* aName)
		{
			aProperty->SetName(aName);
			aParentComponent->AddProperty(aProperty);
		}

		SGF_PropertyDeclHelper(SGF_Property<T>* aProperty, SGF_Component* aParentComponent, const char* aName, const T& aResetValue)
		{
			aProperty->SetName(aName);
			aProperty->SetResetValue(aResetValue);
			aParentComponent->AddProperty(aProperty);
		}
	};

	#define SGF_PROPERTY(aVariableType, aVariableName, aPropertyName, ...)	\
			SGF_Property<aVariableType> aVariableName;		\
			SGF_PropertyDeclHelper<aVariableType> SC_CONCAT(__nHelper, aVariableName) = SGF_PropertyDeclHelper<aVariableType>(&aVariableName, this, aPropertyName, __VA_ARGS__)
#else
	#define SGF_PROPERTY(aVariableType, aVariableName, aPropertyName, ...) SGF_Property<aVariableType> aVariableName;
#endif