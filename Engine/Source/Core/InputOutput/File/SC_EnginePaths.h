#pragma once
#include "SC_File.h"

class SC_EnginePaths
{
public:
	static bool Init(const SC_FilePath& aArgV0, const SC_FilePath& aGameDataDirectory, const SC_FilePath& aEngineDataDirectory);
	static void Destroy();
	static SC_EnginePaths& Get();

public:
	SC_EnginePaths() {}
	~SC_EnginePaths() {}

	// All directory paths will exclude the trailing slash!
	const SC_FilePath& GetWorkingDirectory() const;
	const SC_FilePath& GetGameDataDirectory() const;
	const SC_FilePath& GetEngineDataDirectory() const;

	SC_FilePath GetUserDocumentsDirectory() const;
	SC_FilePath GetUserAppDataDirectory() const;

private:
	static SC_EnginePaths* gInstance;

	SC_FilePath mWorkingDirectory;
	SC_FilePath mGameDataDirectory;
	SC_FilePath mEngineDataDirectory;

};

