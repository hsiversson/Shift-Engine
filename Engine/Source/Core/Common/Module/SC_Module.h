#pragma once

class SC_Module
{
public:
	SC_Module();
	virtual ~SC_Module();

	virtual bool OnRegister() { return true; }
	virtual void OnUnregister() { }

	virtual bool OnLoad() { return true; }
	virtual bool OnUnload() { return true; }

	virtual void OnUpdate() {}
	virtual void OnRender() {}

	virtual const char* Id() const = 0;
	virtual const char* Description() const = 0;

protected:

	virtual const char* _ModuleDependencies() { return nullptr; }
	virtual const char* _ModuleUserDependencies() { return nullptr; }
#if IS_EDITOR_BUILD
	virtual const char* _ModuleEditorDependencies() { return nullptr; }
#endif
};

#define SC_MODULE_DECLARE(aModuleType, aId, aDescription)				\
public:																	\
	friend class SC_Module;												\
	static const char* GetId() { return aId; }							\
	const char* Id() const override { return aId; }						\
	const char* Description() const override { return aDescription; }	\


#define SC_MODULE_DEPENDENCIES(...) \
	const char* _ModuleDependencies() override { return #__VA_ARGS__; }

#define SC_MODULE_USER_DEPENDENCIES(...) \
	const char* _ModuleUserDependencies() override { return #__VA_ARGS__; }

#if IS_EDITOR_BUILD
#define SC_MODULE_EDITOR_DEPENDENCIES(...) \
	const char* _ModuleEditorDependencies() override { return #__VA_ARGS__; }
#else
#define SC_MODULE_EDITOR_DEPENDENCIES(...)
#endif