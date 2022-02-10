#pragma once
#include "SR_ShaderCompileStructs.h"

// For COM interface (ComPtr)
#include <wrl.h>
template<class T>
using SR_ComPtr = Microsoft::WRL::ComPtr<T>;

struct IDxcCompiler3;
struct IDxcUtils;
struct IDxcIncludeHandler;

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
	bool CheckShaderCacheForEntry(const SC_FilePath& aFilePath, SR_ShaderByteCode& aOutResult, SR_ShaderMetaData* aOutMetaData) const;
	bool WriteToCache(const SC_FilePath& aFilePath, const SR_ShaderByteCode& aResult, const SR_ShaderMetaData* aMetaData) const;

private:
	SR_ComPtr<IDxcCompiler3> mDxcCompiler;
	SR_ComPtr<IDxcUtils> mDxcUtils;
	SR_ComPtr<IDxcIncludeHandler> mDxcIncludeHandler;

	Type mCompilerType;
	uint8 mShaderOptimizationLevel;
	bool mSkipOptimizations : 1;
	bool mDebugShaders : 1;
};

