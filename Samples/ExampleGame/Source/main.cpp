#include "Common/SC_Base.h"
#include "InputOutput/File/SC_EnginePaths.h"
#include "Application/SAF_Framework.h"
#include <filesystem>

#if IS_EDITOR_BUILD
#include "SED_Editor.h"
#else
#include "GameInstance.h"
#endif

#if IS_WINDOWS_PLATFORM
int WINAPI WinMain(_In_ HINSTANCE /*hInst*/, _In_opt_ HINSTANCE /*hInstPrev*/, _In_ LPSTR /*aCmdline*/, _In_ int /*aCmdShow*/)
{
	SC_FilePath argv0(__argv[0]);
#else
int main(int aArgC, char* aArgV[])
{
	SC_FilePath argv0(aArgV[0]);
#endif
	argv0 = SC_FilePath::GetParentDirectory(argv0);
	argv0 = SC_FilePath::Normalize(argv0);

	// Init ConfigManager and load config

	// Init EnginePaths
	SC_EnginePaths::Init(argv0, SC_FilePath("Samples/ExampleGame/Data"), SC_FilePath("Engine/Data"));

	int returnCode = 0;
	{
		SAF_FrameworkCreateParams createParams;
#if IS_EDITOR_BUILD
		SED_Editor editorInstance;
		createParams.mCallbacks = &editorInstance;
#else
		GameInstance gameInstance;
		createParams.mCallbacks = &gameInstance;
#endif
		SAF_Framework* instance = SAF_Framework::Create(createParams);
		returnCode = instance->AppMain();
	}

	SAF_Framework::Destroy();
	return returnCode;
}