#pragma once

class SC_Module;
class SC_ModuleManager
{
public:
	static bool Create();
	static void Destroy();
	static SC_ModuleManager* Get();

public:
	SC_ModuleManager();
	~SC_ModuleManager();

	bool RegisterModule(SC_Module* aModuleInstance);
	bool RegisterModule(const char* aModuleId);
	bool UnregisterModule(SC_Module* aModuleInstance);
	bool UnregisterModule(const char* aModuleId);

	bool IsRegistered(const char* aModuleId);

	bool LoadModule(SC_Module* aModuleInstance, bool aLoadDependencies = false);
	bool LoadModule(const char* aModuleId, bool aLoadDependencies = false);
	bool UnloadModule(SC_Module* aModuleInstance);
	bool UnloadModule(const char* aModuleId);

	SC_Module* GetModule(const char* aModuleId);

private:

private:
	static SC_ModuleManager* gInstance;

	SC_UnorderedMap<const char*, SC_Module*> mNamedModules;
	SC_Array<SC_Module*> mModules;
};
