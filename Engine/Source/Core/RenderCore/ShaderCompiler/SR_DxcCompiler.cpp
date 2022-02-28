#include "SR_DxcCompiler.h"

#include "dxc/dxcapi.h"
#include "dxc/DxilContainer/DxilContainer.h"
#include "dxc/Support/dxcapi.use.h"
#include <fstream>
#include <streambuf>
#include <sstream>

static const wchar_t* GetShaderModelVersion()
{
	switch (SR_RenderDevice::gInstance->GetSupportCaps().mHighestShaderModel)
	{
	case SR_ShaderModel::SM_5_1:
		return L"5_1";
	case SR_ShaderModel::SM_6_0:
		return L"6_0";
	case SR_ShaderModel::SM_6_1:
		return L"6_1";
	case SR_ShaderModel::SM_6_2:
		return L"6_2";
	case SR_ShaderModel::SM_6_3:
		return L"6_3";
	case SR_ShaderModel::SM_6_4:
		return L"6_4";
	case SR_ShaderModel::SM_6_5:
		return L"6_5";
	case SR_ShaderModel::SM_6_6:
		return L"6_6";
	case SR_ShaderModel::SM_6_7:
		return L"6_7";
	case SR_ShaderModel::Unknown:
	default:
		return L"6_7";
		//return L"Unknown SM version.";
	}
}

static std::wstring GetTargetProfile(const SR_ShaderType& aShaderType)
{
	std::wstring shaderTarget;
	switch (aShaderType)
	{
	case SR_ShaderType::Vertex:
		shaderTarget = L"vs_";
		break;
#if ENABLE_MESH_SHADERS
	case SR_ShaderType::Amplification:
		shaderTarget = L"as_";
		break;
	case SR_ShaderType::Mesh:
		shaderTarget = L"ms_";
		break;
#endif
	case SR_ShaderType::Pixel:
		shaderTarget = L"ps_";
		break;

	case SR_ShaderType::Compute:
		shaderTarget = L"cs_";
		break;
#if ENABLE_RAYTRACING
	case SR_ShaderType::Raytracing:
		shaderTarget = L"lib_";
		break;
#endif // _DEBUG

	default:
		assert(false);
		return L"Unknown target";
	}

	shaderTarget += GetShaderModelVersion();
	return shaderTarget;
}

bool SR_ShaderCompilerCache::ShaderCacheEntry::ReadShaderData(SR_ShaderByteCode& aOutResult, SR_ShaderMetaData* aOutMetaData)
{
	std::ifstream inStream(mFilePath, std::ios::binary);
	if (!inStream.is_open())
		return false;

	uint64 entryPointSize = 0;
	inStream.read((char*)&entryPointSize, sizeof(uint64));
	aOutResult.mEntryPoint.resize(entryPointSize);
	inStream.read(aOutResult.mEntryPoint.data(), entryPointSize);

	uint64 size = 0;
	inStream.read((char*)&size, sizeof(uint64));

	aOutResult.mSize = size;
	aOutResult.mByteCode = SC_MakeUnique<uint8[]>(size);
	inStream.read((char*)aOutResult.mByteCode.get(), size);

	bool hasMetaData = false;
	inStream.read((char*)&hasMetaData, sizeof(bool));

	if (hasMetaData)
		inStream.read((char*)aOutMetaData, sizeof(SR_ShaderMetaData));

	return true;
}

bool SR_ShaderCompilerCache::ShaderCacheEntry::WriteShaderData(const SR_ShaderByteCode& aOutResult, const SR_ShaderMetaData* aOutMetaData)
{
	SC_FilePath::CreateDirectory(gShaderCacheFolder);
	std::ofstream outStream(mFilePath, std::ios::binary);
	if (!outStream.is_open())
		return false;

	uint64 entryPointStrSize = aOutResult.mEntryPoint.length();
	outStream.write((const char*)&entryPointStrSize, sizeof(uint64));
	outStream.write(aOutResult.mEntryPoint.data(), entryPointStrSize);

	outStream.write((const char*)&aOutResult.mSize, sizeof(uint64));
	outStream.write((const char*)aOutResult.mByteCode.get(), aOutResult.mSize);

	bool hasMetaData = (aOutMetaData != nullptr);
	outStream.write((const char*)&hasMetaData, sizeof(bool));

	if (hasMetaData)
		outStream.write((const char*)aOutMetaData, sizeof(SR_ShaderMetaData));

	return true;
}

SR_ShaderCompilerCache::SR_ShaderCompilerCache()
{

}

SR_ShaderCompilerCache::~SR_ShaderCompilerCache()
{

}

SR_ShaderCompilerCache::QueryResult SR_ShaderCompilerCache::QueryShaderEntry(const uint64 aHash, ShaderCacheEntry& aOutShaderCacheEntry)
{
	std::string hashedFileName;
	{
		std::stringstream ss;
		ss << std::hex << aHash;
		hashedFileName = ss.str();
	}
	hashedFileName += ".bytecode";

	aOutShaderCacheEntry.mFilePath = SC_FormatStr("%s/%s", gShaderCacheFolder, hashedFileName.c_str());

	if (SC_FilePath::Exists(aOutShaderCacheEntry.mFilePath.c_str()))
		return QueryResult::Found;
	else
		return QueryResult::Missing;
}

SR_DxcCompiler::SR_DxcCompiler(const Type& aType)
	: mCompilerType(aType)
	, mShaderOptimizationLevel(3)
	, mSkipOptimizations(false)
	, mDebugShaders(false)
{
	if (SC_CommandLine::HasCommand("debugshaders"))
		mDebugShaders = true;

	if (SC_CommandLine::HasCommand("skipshaderoptimizations"))
		mSkipOptimizations = true;

	if (!mSkipOptimizations)
	{
		if (SC_CommandLine::HasArgument("shaderoptimizationlevel", "0"))
			mShaderOptimizationLevel = 0;
		else if (SC_CommandLine::HasArgument("shaderoptimizationlevel", "1"))
			mShaderOptimizationLevel = 1;
		else if (SC_CommandLine::HasArgument("shaderoptimizationlevel", "2"))
			mShaderOptimizationLevel = 2;
		else if (SC_CommandLine::HasArgument("shaderoptimizationlevel", "3"))
			mShaderOptimizationLevel = 3;
	}

	HRESULT result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&mDxcCompiler));
	//if (FAILED(result))
	//	LOG_ERROR("Failed to create compiler DXC");

	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&mDxcUtils));
	//if (FAILED(result))
	//	LOG_ERROR("Failed to create instance DXC");

	result = mDxcUtils->CreateDefaultIncludeHandler(&mDxcIncludeHandler);
	//if (FAILED(result))
	//	LOG_ERROR("Failed to create default include handler DXC");

}

SR_DxcCompiler::~SR_DxcCompiler()
{

}

bool SR_DxcCompiler::CompileFromFile(const SR_ShaderCompileArgs& aArgs, SR_ShaderByteCode& aOutResult, SR_ShaderMetaData* aOutMetaData)
{
	std::ifstream t(aArgs.mShaderFile.GetAbsolutePath());
	std::string shaderCodeBuffer((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	return CompileFromString(shaderCodeBuffer, aArgs, aOutResult, aOutMetaData);
}

bool SR_DxcCompiler::CompileFromString(const std::string& aShadercode, const SR_ShaderCompileArgs& aArgs, SR_ShaderByteCode& aOutResult, SR_ShaderMetaData* aOutMetaData)
{
	const std::wstring targetProfile = GetTargetProfile(aArgs.mType);

	std::string shaderCode(aShadercode);
	shaderCode += "/*";
	shaderCode += SC_UTF16ToUTF8(targetProfile.c_str());
	if (mCompilerType == Type::SPIRV)
		shaderCode += "Bytecode: SPIR-V";
	else
		shaderCode += "Bytecode: DXIL";
	shaderCode += "*/";

	uint64 shaderHash;
	{
		std::hash<std::string> hasher;
		shaderHash = (uint64)hasher(shaderCode);
	}

	SR_ShaderCompilerCache::ShaderCacheEntry cacheEntry;
	SR_ShaderCompilerCache::QueryResult queryResult = mShaderCache.QueryShaderEntry(shaderHash, cacheEntry);
	if (queryResult == SR_ShaderCompilerCache::QueryResult::Found)
	{
		return cacheEntry.ReadShaderData(aOutResult, aOutMetaData);
	}
	else
	{
		DxcBuffer sourceBuffer;
		sourceBuffer.Ptr = shaderCode.data();
		sourceBuffer.Size = shaderCode.size();
		sourceBuffer.Encoding = DXC_CP_ACP;

		SC_Array<LPCWSTR> compileArgs;
		compileArgs.Reserve(64);

		if (mCompilerType == Type::SPIRV)
		{
			compileArgs.Add(L"-spirv");
			compileArgs.Add(L"-fvk-use-dx-layout");
			compileArgs.Add(L"-fvk-use-dx-position-w");
		}

		compileArgs.Add(L"-HV 2021"); // TEMPORARY UNTIL HLSL 2021 BECOMES DEFAULT

		compileArgs.Add(L"-E");
		std::wstring entryPoint = SC_UTF8ToUTF16(aArgs.mEntryPoint);
		compileArgs.Add(entryPoint.c_str());

		compileArgs.Add(L"-T");
		compileArgs.Add(targetProfile.c_str());

		// Debug Info
		if (mDebugShaders)
		{
			compileArgs.Add(L"-Zi");
			compileArgs.Add(L"-Zss");
		}
		else
			compileArgs.Add(L"-Qstrip_debug");

		if (mSkipOptimizations)
			compileArgs.Add(L"-Od");
		else
		{
			if (mShaderOptimizationLevel == 2)
				compileArgs.Add(L"-O2");
			else if (mShaderOptimizationLevel == 1)
				compileArgs.Add(L"-O1");
			else if (mShaderOptimizationLevel == 0)
				compileArgs.Add(L"-O0");
			else
				compileArgs.Add(L"-O3");
		}

		compileArgs.Add(L"-D");
		if (mCompilerType == Type::SPIRV)
			compileArgs.Add(L"IS_SPIRV=1");
		else
			compileArgs.Add(L"IS_DXIL=1");

		SC_Array<std::wstring> rawStrDefines;
		for (uint32 i = 0; i < aArgs.mDefines.Count(); ++i)
		{
			compileArgs.Add(L"-D");

			std::wstring& define = rawStrDefines.Add(SC_UTF8ToUTF16(aArgs.mDefines[i].mFirst.c_str()));
			define += L"=";
			define += SC_UTF8ToUTF16(aArgs.mDefines[i].mSecond.c_str());

			compileArgs.Add(define.c_str());
		}

		compileArgs.Add(L"/I");

		SC_FilePath includeDir = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders";
		std::wstring includeDirStr = SC_UTF8ToUTF16(includeDir.GetStr());
		compileArgs.Add(includeDirStr.c_str());

		if (!aOutMetaData)
			compileArgs.Add(L"-Qstrip_reflect");

		compileArgs.Add(L"-flegacy-macro-expansion");
		compileArgs.Add(L"-all_resources_bound");

		SR_ComPtr<IDxcResult> compileResults;
		HRESULT result = mDxcCompiler->Compile(&sourceBuffer, compileArgs.GetBuffer(), compileArgs.Count(), mDxcIncludeHandler.Get(), IID_PPV_ARGS(&compileResults));
		if (compileResults->HasOutput(DXC_OUT_ERRORS))
		{
			SR_ComPtr<IDxcBlobUtf8> errors;
			SR_ComPtr<IDxcBlobUtf16> errorsOutputName;
			result = compileResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), &errorsOutputName);

			if (errors && errors->GetStringLength() > 0)
			{
				SC_ERROR("Shader compilation error: %s", errors->GetStringPointer());
				return false;
			}
		}

		if (compileResults->HasOutput(DXC_OUT_OBJECT))
		{
			SR_ComPtr<IDxcBlob> shaderByteCode;
			SR_ComPtr<IDxcBlobUtf16> outputName;
			result = compileResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderByteCode), &outputName);
			if (SUCCEEDED(result))
			{
				aOutResult.mSize = static_cast<uint64>(shaderByteCode->GetBufferSize());
				aOutResult.mByteCode = SC_MakeUnique<uint8[]>(aOutResult.mSize);
				SC_Memcpy(aOutResult.mByteCode.get(), shaderByteCode->GetBufferPointer(), aOutResult.mSize);
			}
			else
				return false;
		}
		else
			return false;

		if (aOutMetaData && compileResults->HasOutput(DXC_OUT_REFLECTION))
		{
			SR_ComPtr<IDxcBlob> reflectionData;
			SR_ComPtr<IDxcBlobUtf16> outputName;
			result = compileResults->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflectionData), &outputName);
			if (SUCCEEDED(result))
			{
				DxcBuffer reflectionBuffer;
				reflectionBuffer.Ptr = reflectionData->GetBufferPointer();
				reflectionBuffer.Size = reflectionData->GetBufferSize();
				reflectionBuffer.Encoding = 0;

#if ENABLE_DX12
				SR_ComPtr<ID3D12ShaderReflection> reflection;
				mDxcUtils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(&reflection));
				if (reflection)
				{
					//D3D12_SHADER_DESC shaderDesc = {};
					//result = reflection->GetDesc(&shaderDesc);
					//if (FAILED(result))
					//	return false;

					//shaderDesc.

					uint32 threads[3];
					reflection->GetThreadGroupSize(&threads[0], &threads[1], &threads[2]);
					aOutMetaData->mNumThreads = SC_IntVector(threads[0], threads[1], threads[2]);
				}
#endif
			}
			else
				return false;
		}

		return cacheEntry.WriteShaderData(aOutResult, aOutMetaData);
	}
}
