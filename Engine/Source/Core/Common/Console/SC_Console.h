#pragma once

class SC_TypedCVarBase
{
	friend class SC_Console;
public:
	virtual ~SC_TypedCVarBase() {}
	virtual void Register() = 0;
};

class SC_Console
{
private:
	struct ConsoleVariable
	{
		enum class Type
		{
			Bool,
			Int8,
			Int16,
			Int32,
			Int64,
			Uint8,
			Uint16,
			Uint32,
			Uint64,
			Float,
			Double,
			Vector2,
			IntVector2,
			Vector3,
			IntVector3,
			Vector4,
			IntVector4,
			Unknown
		};

		ConsoleVariable() : mCVarPtr(nullptr), mFlags(0), mType(Type::Unknown) {}
		ConsoleVariable(const char* aName, void* aCVar, const Type& aType, uint32 aFlags, const char* aLayout = nullptr)
			: mName(aName)
			, mLayout(aLayout)
			, mCVarPtr(aCVar)
			, mFlags(aFlags)
			, mType(aType)
		{}

		std::string mName;
		std::string mLayout;

		void* mCVarPtr;
		uint32 mFlags;
		Type mType;
	};

public:
	static bool Create();
	static void Destroy();
	static SC_Console* Get();

	SC_Console();
	~SC_Console();

	bool Init();

	void AddVariable(const char* aName, bool& aBool, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, int8& aInt8, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, int16& aInt16, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, int32& aInt32, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, int64& aInt64, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, uint8& aUint8, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, uint16& aUint16, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, uint32& aUint32, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, uint64& aUint64, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, float& aFloat, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, double& aDouble, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, SC_Vector2& aVector, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, SC_Vector& aVector, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, SC_Vector4& aVector, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, SC_IntVector2& aVector, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, SC_IntVector& aVector, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, SC_IntVector4& aVector, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	void AddVariable(const char* aName, void* aCVarPtr, const ConsoleVariable::Type& aType, uint32 aFlags = 0, const std::string& aInputLayout = std::string(""));
	//void AddVariable(const char* aName, MC_String& aString, int someFlags = MC_CONSOLEVARFLAG_READ | MC_CONSOLEVARFLAG_WRITE, const MC_String& anInputLayout = MC_String::Empty);
	//void AddVariable(const char* aName, MC_Tribool& aTribool, int someFlags = MC_CONSOLEVARFLAG_READ | MC_CONSOLEVARFLAG_WRITE, const MC_String& anInputLayout = MC_String::Empty);
	//void AddVariable(void* aVar, MC_ConsoleVarType aType, const char* aName, int someFlags = MC_CONSOLEVARFLAG_READ | MC_CONSOLEVARFLAG_WRITE, const MC_String& anInputLayout = MC_String::Empty);

	void Update();

	static SC_Array<SC_TypedCVarBase*>& GetAddQueue();

private:
	SC_UnorderedMap<std::string, ConsoleVariable> mVariables;

	static SC_Console* gInstance;
};

template<class T>
class SC_TypedCVar : public SC_TypedCVarBase
{
public:
	SC_TypedCVar(const char* name, const T& defaultValue)
		: mName(name)
		, mValue(defaultValue)
	{
		Register();
	}

	void Register() override
	{
		SC_Console* console = SC_Console::Get();
		if (console)
		{
			console->AddVariable(mName, mValue);
		}
		else
		{
			SC_Console::GetAddQueue().Add(this);
		}
	}

	operator T() const { return mValue; }
	operator T& () { return mValue; }
	bool operator==(const T& aOther) const { return mValue == aOther; }
	bool operator!=(const T& aOther) const { return mValue != aOther; }

	T& operator=(const T& aOther)
	{
		mValue = aOther;
		return mValue;
	}

private:
	const char* mName;
	T mValue;
};

#if ENABLE_CONSOLE
#define SC_CVAR(aType, aVariableName, aCommandName, aDefaultVal) static SC_TypedCVar<aType> aVariableName("" aCommandName, aDefaultVal)
#define SC_CONSOLE_FUNC()
#else
#define SC_CVAR(aType, aVariableName, aCommandName, aDefaultVal) static aType aVariableName = aDefaultVal
#define SC_CONSOLE_FUNC()
#endif