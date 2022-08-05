
#pragma once
#include "SSC_Defines.h"

#if SSC_ENABLE_MONO_SCRIPT

typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoDomain MonoDomain;

class SSC_ScriptAssembly_Mono
{
	friend class SSC_ScriptCore_Mono;
public:
	SSC_ScriptAssembly_Mono();
	~SSC_ScriptAssembly_Mono();

	bool Load(const char* aScriptFile);

private:
	MonoAssembly* mAssembly;
};

class SSC_ScriptCore_Mono
{
public:
	SSC_ScriptCore_Mono();
	~SSC_ScriptCore_Mono();

	bool Init();

private:
	MonoDomain* mRootDomain;
	MonoDomain* mAppDomain;
};

#endif