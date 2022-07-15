#pragma once

#if SR_ENABLE_DIRECTSTORAGE

struct IDStorageFactory;
struct IDStorageFile;
struct IDStorageQueue;
struct DSTORAGE_REQUEST;

class SR_FileLoader_DirectStorage
{
public:
	enum RequestType
	{
		RequestType_File,
		RequestType_Memory,
		RequestType_Count,
	};

public:
	SR_FileLoader_DirectStorage();
	~SR_FileLoader_DirectStorage();

	bool Init();

	bool LoadFromFile(const char* aPath);
	bool LoadFromMemory(const char* aPath);

private:
	bool LoadInternal(DSTORAGE_REQUEST& aRequestDesc);


	SR_ComPtr<IDStorageFactory> mFactory;
	SR_ComPtr<IDStorageQueue> mQueue[RequestType_Count];
};

#endif