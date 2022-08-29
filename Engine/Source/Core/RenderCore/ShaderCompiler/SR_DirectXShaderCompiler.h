#pragma once
#include "SR_ShaderCompileStructs.h"

// For COM interface (ComPtr)
#include <wrl.h>
template<class T>
using SR_ComPtr = Microsoft::WRL::ComPtr<T>;

struct IDxcCompiler3;
struct IDxcUtils;
struct IDxcIncludeHandler;

class SR_ShaderCompilerCache
{
public:
	enum class QueryResult
	{
		Found,
		Missing,
		Error
	};

	struct ShaderCacheEntry
	{
		friend class SR_ShaderCompilerCache;
	public:
		bool ReadShaderData(SR_ShaderByteCode& aOutResult, SR_ShaderMetaData* aOutMetaData = nullptr);
		bool WriteShaderData(const SR_ShaderByteCode& aResult, const SR_ShaderMetaData* aMetaData);

		const char* GetFileName() const;

	private:
		std::string mFileName;
		std::string mFilePath;
	};

public:
	SR_ShaderCompilerCache();
	~SR_ShaderCompilerCache();

	QueryResult QueryShaderEntry(const uint64 aHash, ShaderCacheEntry& aOutShaderCacheEntry);

private:
	static constexpr const char* gShaderCacheFolder = "Cache/ShaderCache";
};

enum SR_CompilerFlags
{
	SR_CompilerFlag_None = 0,
	SR_CompilerFlag_Debug = 1 << 0,
	SR_CompilerFlag_SkipOptimizations = 1 << 1,
	SR_CompilerFlag_Enable16BitTypes = 1 << 2,
	SR_CompilerFlag_EnablePayloadQualifiers = 1 << 3,
	SR_CompilerFlag_IEEEStrictness = 1 << 4,
	//SR_CompilerFlag_Debug = 1 << 2,
	//SR_CompilerFlag_Debug = 1 << 3,
	//SR_CompilerFlag_Debug = 1 << 4,
};

typedef HRESULT(__stdcall* DxcCreateInstanceProc)(
	_In_ REFCLSID   rclsid,
	_In_ REFIID     riid,
	_Out_ LPVOID* ppv
	);

class SR_DxcIncludeHandler;
class SR_DirectXShaderCompiler
{
public:
	enum class Backend
	{
		DXIL,
		SPIRV
	};

public:
	SR_DirectXShaderCompiler(const Backend& aBackend = Backend::DXIL);
	~SR_DirectXShaderCompiler();

	bool CompileFromFile(const SR_ShaderCompileArgs& aArgs, SR_ShaderByteCode& aOutResult, SR_ShaderMetaData* aOutMetaData = nullptr);
	bool CompileFromString(const std::string& aShadercode, const SR_ShaderCompileArgs& aArgs, SR_ShaderByteCode& aOutResult, SR_ShaderMetaData* aOutMetaData = nullptr, const SC_FilePath& aBaseDirectory = SC_FilePath());

private:
	void SetupArgs(SC_Array<LPCWSTR>& aOutArgs, uint32 aCompilerFlags);

	SR_ShaderCompilerCache mShaderCache;

	DxcCreateInstanceProc mDxcCreateInstance;

	Backend mCompilerBackend;
	uint8 mShaderOptimizationLevel;
	bool mSkipOptimizations : 1;
	bool mDebugShaders : 1;
};

