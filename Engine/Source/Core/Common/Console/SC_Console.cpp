#include "SC_Console.h"

SC_Console* SC_Console::gInstance = nullptr;

bool SC_Console::Create()
{
	if (!gInstance)
	{
		gInstance = new SC_Console;
		if (!gInstance->Init())
		{
			delete gInstance;
			gInstance = nullptr;
			return false;
		}

		return true;
	}

	return false;
}

void SC_Console::Destroy()
{
	SC_Console* instance = gInstance;
	gInstance = nullptr;
	delete instance;
}

SC_Console* SC_Console::Get()
{
	return gInstance;
}

SC_Console::SC_Console()
{
}

SC_Console::~SC_Console()
{

}

bool SC_Console::Init()
{
	SC_Array<SC_TypedCVarBase*>& addQueue = GetAddQueue();
	for (SC_TypedCVarBase* cvar : addQueue)
	{
		cvar->Register();
	}
	addQueue.RemoveAll();

	return true;
}

void SC_Console::AddVariable(const char* aName, bool& aBool, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aBool, ConsoleVariable::Type::Bool, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, int8& aInt8, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aInt8, ConsoleVariable::Type::Int8, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, int16& aInt16, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aInt16, ConsoleVariable::Type::Int16, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, int32& aInt32, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aInt32, ConsoleVariable::Type::Int32, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, int64& aInt64, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aInt64, ConsoleVariable::Type::Int64, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, uint8& aUint8, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aUint8, ConsoleVariable::Type::Uint8, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, uint16& aUint16, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aUint16, ConsoleVariable::Type::Uint16, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, uint32& aUint32, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aUint32, ConsoleVariable::Type::Uint32, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, uint64& aUint64, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aUint64, ConsoleVariable::Type::Uint64, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, float& aFloat, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aFloat, ConsoleVariable::Type::Float, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, double& aDouble, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aDouble, ConsoleVariable::Type::Double, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, SC_Vector2& aVector, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aVector, ConsoleVariable::Type::Vector2, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, SC_Vector& aVector, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aVector, ConsoleVariable::Type::IntVector2, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, SC_Vector4& aVector, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aVector, ConsoleVariable::Type::Vector3, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, SC_IntVector2& aVector, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aVector, ConsoleVariable::Type::IntVector3, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, SC_IntVector& aVector, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aVector, ConsoleVariable::Type::Vector4, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, SC_IntVector4& aVector, uint32 aFlags /*= 0*/, const std::string& aInputLayout /*= std::string("")*/)
{
	AddVariable(aName, &aVector, ConsoleVariable::Type::IntVector4, aFlags, aInputLayout);
}

void SC_Console::AddVariable(const char* aName, void* aCVarPtr, const ConsoleVariable::Type& aType, uint32 aFlags, const std::string& aInputLayout)
{
	std::string lowerCaseName = aName;
	SC_ToLower(lowerCaseName);
	mVariables[lowerCaseName] = ConsoleVariable(aName, aCVarPtr, aType, aFlags, aInputLayout.c_str());
}

void SC_Console::Update()
{
}

SC_Array<SC_TypedCVarBase*>& SC_Console::GetAddQueue()
{
	static SC_Array<SC_TypedCVarBase*> gAddQueue;
	return gAddQueue;
}
