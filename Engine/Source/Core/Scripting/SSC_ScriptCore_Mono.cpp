#include "SSC_ScriptCore_Mono.h"

#if SSC_ENABLE_MONO_SCRIPT
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/image.h"
#include "mono/utils/mono-forward.h"

SSC_ScriptAssembly_Mono::SSC_ScriptAssembly_Mono()
	: mAssembly(nullptr)
{
}

SSC_ScriptAssembly_Mono::~SSC_ScriptAssembly_Mono()
{
	mono_assembly_close(mAssembly);
}

bool SSC_ScriptAssembly_Mono::Load(const char* aScriptFile)
{
	char* fileData = nullptr;
	uint32 fileDataSize = 0;


	MonoImageOpenStatus status;
	MonoImage* image = mono_image_open_from_data_full(fileData, fileDataSize, 1, &status, 0);
	if (status != MONO_IMAGE_OK)
	{
		SC_ERROR("Failed to load mono script. Error: {}", mono_image_strerror(status));
		return false;
	}

	mAssembly = mono_assembly_load_from_full(image, aScriptFile, &status, 0);
	
	mono_image_close(image);
	return false;
}

SSC_ScriptCore_Mono::SSC_ScriptCore_Mono()
	: mRootDomain(nullptr)
	, mAppDomain(nullptr)
{

}

SSC_ScriptCore_Mono::~SSC_ScriptCore_Mono()
{
	mono_domain_unload(mAppDomain);
	mono_jit_cleanup(mRootDomain);
}

bool SSC_ScriptCore_Mono::Init()
{
	mRootDomain = mono_jit_init("ShiftJITRuntime");
	if (!mRootDomain)
	{
		SC_ERROR("Failed to initialize Mono Root Domain.");
		return false;
	}

	mAppDomain = mono_domain_create_appdomain((char*)"ShiftScriptRuntime", nullptr);
	mono_domain_set(mAppDomain, true);

	SSC_ScriptAssembly_Mono assembly;
	SC_FilePath path(SC_EnginePaths::Get().GetGameDataDirectory());
	path += "/Scripts/Shift-ScriptCore.dll";
	assembly.Load(path.GetStr());

	return true;
}

#endif
