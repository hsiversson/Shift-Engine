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

	private:
		std::string mFilePath;
	};

public:
	SR_ShaderCompilerCache();
	~SR_ShaderCompilerCache();

	QueryResult QueryShaderEntry(const uint64 aHash, ShaderCacheEntry& aOutShaderCacheEntry);

private:
	static constexpr const char* gShaderCacheFolder = "Cache/ShaderCache";
};

class SR_DxcCompiler
{
public:
	enum class Type
	{
		DXIL,
		SPIRV
	};

public:
	SR_DxcCompiler(const Type& aType = Type::DXIL);
	~SR_DxcCompiler();

	bool CompileFromFile(const SR_ShaderCompileArgs& aArgs, SR_ShaderByteCode& aOutResult, SR_ShaderMetaData* aOutMetaData = nullptr);
	bool CompileFromString(const std::string& aShadercode, const SR_ShaderCompileArgs& aArgs, SR_ShaderByteCode& aOutResult, SR_ShaderMetaData* aOutMetaData = nullptr);

private:
	SR_ComPtr<IDxcCompiler3> mDxcCompiler;
	SR_ComPtr<IDxcUtils> mDxcUtils;
	SR_ComPtr<IDxcIncludeHandler> mDxcIncludeHandler;

	SR_ShaderCompilerCache mShaderCache;

	Type mCompilerType;
	uint8 mShaderOptimizationLevel;
	bool mSkipOptimizations : 1;
	bool mDebugShaders : 1;
};

