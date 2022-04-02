#pragma once

class SR_Texture;
class SGfx_MeshInstance;
class SGfx_MaterialInstance;
class SGF_Entity;

class SGF_PropertyHelperBase;
class SGF_PropertyContainer
{
public:
	SGF_PropertyContainer() {}
	virtual ~SGF_PropertyContainer() {}

#if IS_EDITOR_BUILD
	void AddProperty(SGF_PropertyHelperBase* aProperty) { mProperties.Add(aProperty); }
	const SC_Array<SGF_PropertyHelperBase*>& GetProperties() const { return mProperties; }
#endif

protected:
#if IS_EDITOR_BUILD
	SC_Array<SGF_PropertyHelperBase*> mProperties;
#endif
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
	SGF_PropertyHelper(SGF_PropertyContainer* aParent, const char* aName, T& aProperty)
		: SGF_PropertyHelperBase(SGF_PropertyHelperBase::GetTypeFromClassType<T>(), aName)
		, mPropertyRef(aProperty)
	{
		if (aParent)
			aParent->AddProperty(this);
	}

	SGF_PropertyHelper(SGF_PropertyContainer* aParent, const char* aName, T& aProperty, const char* aMetaData)
		: SGF_PropertyHelperBase(SGF_PropertyHelperBase::GetTypeFromClassType<T>(), aName, aMetaData)
		, mPropertyRef(aProperty)
	{
		if (aParent)
			aParent->AddProperty(this);
	}

	T& Get() { return mPropertyRef; }
	const T& Get() const { return mPropertyRef; }

private:
	T& mPropertyRef;
};

#define SGF_PROPERTY(aVariableType, aVariableName, aPropertyName, ...)	\
			aVariableType aVariableName;		\
			SGF_PropertyHelper<aVariableType> SC_CONCAT(__nHelper, aVariableName) = SGF_PropertyHelper<aVariableType>(this, aPropertyName, aVariableName, __VA_ARGS__)
#else
#define SGF_PROPERTY(aVariableType, aVariableName, aPropertyName, ...) aVariableType aVariableName;
#endif