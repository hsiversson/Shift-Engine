#include "SC_EnginePaths.h"
#include "EngineTypes/String/SC_StringHelpers.h"

#include <filesystem>

#if IS_WINDOWS_PLATFORM
#include <ShlObj.h>
#endif

SC_EnginePaths* SC_EnginePaths::gInstance = nullptr;

bool SC_EnginePaths::Init(const SC_FilePath& aArgV0, const SC_FilePath& aGameDataDirectory, const SC_FilePath& aEngineDataDirectory)
{
	if (!gInstance)
	{
		gInstance = new SC_EnginePaths;

		std::filesystem::current_path(std::filesystem::path(aArgV0.GetStr()));
		gInstance->mWorkingDirectory = aArgV0;

		// Build relative paths to data directories

		SC_FilePath gameDataDir(aGameDataDirectory);
		SC_FilePath engineDataDir(aEngineDataDirectory);

		bool foundGameDataDir = false;
		bool foundEngineDataDir = false;
		uint32 iterations = 0;
		while (iterations < 4 || (!foundGameDataDir && !foundEngineDataDir))
		{
			if (!foundGameDataDir && SC_FilePath::Exists(gameDataDir))
			{
				gInstance->mGameDataDirectory = gameDataDir;
				foundGameDataDir = true;
			}
			else
				gameDataDir = SC_FilePath("../") + gameDataDir;

			if (!foundEngineDataDir && SC_FilePath::Exists(engineDataDir))
			{
				gInstance->mEngineDataDirectory = engineDataDir;
				foundEngineDataDir = true;
			}
			else
				engineDataDir = SC_FilePath("../") + engineDataDir;

			++iterations;
		}

		return foundGameDataDir && foundEngineDataDir;
	}
	else
		return false;
}

void SC_EnginePaths::Destroy()
{
	delete gInstance;
	gInstance = nullptr;
}

SC_EnginePaths& SC_EnginePaths::Get()
{
	return *gInstance;
}

const SC_FilePath& SC_EnginePaths::GetWorkingDirectory() const
{
	return mWorkingDirectory;
}

const SC_FilePath& SC_EnginePaths::GetGameDataDirectory() const
{
	return mGameDataDirectory;
}

const SC_FilePath& SC_EnginePaths::GetEngineDataDirectory() const
{
	return mEngineDataDirectory;
}

SC_FilePath SC_EnginePaths::GetUserDocumentsDirectory() const
{
	SC_FilePath documentsPath;
#if IS_WINDOWS_PLATFORM

	PWSTR path = NULL;
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path);
	if (SUCCEEDED(hr))
	{
		documentsPath = SC_UTF16ToUTF8(path).c_str();
	}
	CoTaskMemFree(path);

#else
#error Platform not supported yet!
#endif

	return documentsPath;
}

SC_FilePath SC_EnginePaths::GetUserAppDataDirectory() const
{
	SC_FilePath appDataPath;
#if IS_WINDOWS_PLATFORM

	PWSTR path = NULL;
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, &path);
	if (SUCCEEDED(hr))
	{
		appDataPath = SC_UTF16ToUTF8(path).c_str();
	}
	CoTaskMemFree(path);

#else
#error Platform not supported yet!
#endif

	return appDataPath;
}
