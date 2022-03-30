#include "SC_ModuleManager.h"

SC_ModuleManager* SC_ModuleManager::gInstance = nullptr;

bool SC_ModuleManager::Create()
{
	if (gInstance)
		return true;

	gInstance = new SC_ModuleManager();
	if (!gInstance)
		return false;

	return true;
}

void SC_ModuleManager::Destroy()
{
	if (gInstance)
	{
		delete gInstance;
		gInstance = nullptr;
	}
}

SC_ModuleManager* SC_ModuleManager::Get()
{
	return gInstance;
}

SC_ModuleManager::SC_ModuleManager()
{

}

SC_ModuleManager::~SC_ModuleManager()
{

}

bool SC_ModuleManager::RegisterModule(SC_Module* /*aModuleInstance*/)
{
	return true;
}

bool SC_ModuleManager::RegisterModule(const char* /*aModuleId*/)
{
	return true;
}

bool SC_ModuleManager::UnregisterModule(SC_Module* /*aModuleInstance*/)
{
	return true;
}

bool SC_ModuleManager::UnregisterModule(const char* /*aModuleId*/)
{
	return true;
}

bool SC_ModuleManager::IsRegistered(const char* /*aModuleId*/)
{
	return true;
}

bool SC_ModuleManager::LoadModule(SC_Module* /*aModuleInstance*/, bool /*aLoadDependencies*/ /*= false*/)
{
	return true;
}

bool SC_ModuleManager::LoadModule(const char* /*aModuleId*/, bool /*aLoadDependencies*/ /*= false*/)
{
	return true;
}

bool SC_ModuleManager::UnloadModule(SC_Module* /*aModuleInstance*/)
{
	return true;
}

bool SC_ModuleManager::UnloadModule(const char* /*aModuleId*/)
{
	return true;
}

SC_Module* SC_ModuleManager::GetModule(const char* /*aModuleId*/)
{
	return nullptr;
}
