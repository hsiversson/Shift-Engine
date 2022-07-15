#include "SR_DirectXShaderCompiler.h"

#include "dxc/dxcapi.h"
#include <fstream>
#include <streambuf>
#include <sstream>
#include <unordered_set>

class SR_DxcIncludeHandler : public IDxcIncludeHandler
{
public:
	explicit SR_DxcIncludeHandler(IDxcUtils* aDxcUtils)
		: mDxcUtils(aDxcUtils)
		, gBaseDirectory(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders")
	{
	}

	HRESULT STDMETHODCALLTYPE LoadSource(_In_ LPCWSTR aFilename, _COM_Outptr_result_maybenull_ IDxcBlob** aIncludeSource) override
	{
		*aIncludeSource = nullptr;

		SC_FilePath path = gBaseDirectory;
		path += SC_UTF16ToUTF8(aFilename);
		path.MakeAbsolute();
		path = SC_FilePath::Normalize(path);

		SR_ComPtr<IDxcBlobEncoding> encoding;
		if (mIncludedFiles.find(path.GetStr()) != mIncludedFiles.end()) // Already included
		{
			static constexpr const char* nullStr = " ";
			mDxcUtils->CreateBlob(nullStr, 2, 0, encoding.GetAddressOf());
			*aIncludeSource = encoding.Detach();
			return S_OK;
		}
		else
		{
			// Recursively add includes
			std::wstring wpath = SC_UTF8ToUTF16(path.GetStr());
			HRESULT hr = mDxcUtils->LoadFile(wpath.c_str(), nullptr, encoding.GetAddressOf());
			if (SUCCEEDED(hr))
			{
				mIncludedFiles.insert(path.GetStr());
				*aIncludeSource = encoding.Detach();
			}
			else
			{
				if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
					SC_ERROR("File not found: {}", path.GetStr());
			}
			return hr;
		}
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override
	{
		(void)riid;
		(void)ppvObject;
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef(void) override { return 0; }
	ULONG STDMETHODCALLTYPE Release(void) override { return 0; }

private:
	std::unordered_set<std::string> mIncludedFiles;
	IDxcUtils* mDxcUtils;
	const SC_FilePath gBaseDirectory;
};

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
#if SR_ENABLE_MESH_SHADERS
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
#if SR_ENABLE_RAYTRACING
	case SR_ShaderType::Raytracing:
		shaderTarget = L"lib_";
		break;
#endif // _DEBUG

	default:
		SC_ASSERT(false);
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

const char* SR_ShaderCompilerCache::ShaderCacheEntry::GetFileName() const
{
	return mFileName.c_str();
}

SR_ShaderCompilerCache::SR_ShaderCompilerCache()
{

}

SR_ShaderCompilerCache::~SR_ShaderCompilerCache()
{

}

SR_ShaderCompilerCache::QueryResult SR_ShaderCompilerCache::QueryShaderEntry(const uint64 aHash, ShaderCacheEntry& aOutShaderCacheEntry)
{
	{
		std::stringstream ss;
		ss << std::hex << aHash;
		aOutShaderCacheEntry.mFileName = ss.str();
	}
	aOutShaderCacheEntry.mFilePath = SC_FormatStr("{}/{}.bytecode", gShaderCacheFolder, aOutShaderCacheEntry.mFileName.c_str());

	if (SC_FilePath::Exists(aOutShaderCacheEntry.mFilePath.c_str()))
		return QueryResult::Found;
	else
		return QueryResult::Missing;
}

SR_DirectXShaderCompiler::SR_DirectXShaderCompiler(const Backend& aBackend)
	: mCompilerBackend(aBackend)
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
	
	const wchar_t* dllPath = nullptr;
	switch (mCompilerBackend)
	{
	case SR_DirectXShaderCompiler::Backend::DXIL:
	case SR_DirectXShaderCompiler::Backend::SPIRV:
		dllPath = L"dxcompiler.dll";
		break;
	default:
		SC_ASSERT(false, "Invalid Compiler backend specified.");
		break;
	}

	HMODULE hModule = LoadLibrary(dllPath);
	mDxcCreateInstance = (DxcCreateInstanceProc)GetProcAddress(hModule, "DxcCreateInstance");
	if (!mDxcCreateInstance)
	{
		SC_ASSERT(false, "Unable to find DxcCreateInstance entry in dll!");
	}
}

SR_DirectXShaderCompiler::~SR_DirectXShaderCompiler()
{

}

bool SR_DirectXShaderCompiler::CompileFromFile(const SR_ShaderCompileArgs& aArgs, SR_ShaderByteCode& aOutResult, SR_ShaderMetaData* aOutMetaData)
{
	std::ifstream t(aArgs.mShaderFile.GetAbsolutePath());
	std::string shaderCodeBuffer((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	return CompileFromString(shaderCodeBuffer, aArgs, aOutResult, aOutMetaData);
}

bool SR_DirectXShaderCompiler::CompileFromString(const std::string& aShadercode, const SR_ShaderCompileArgs& aArgs, SR_ShaderByteCode& aOutResult, SR_ShaderMetaData* aOutMetaData)
{
	const std::wstring targetProfile = GetTargetProfile(aArgs.mType);

	std::string shaderCode(aShadercode);
	shaderCode += SC_FormatStr("// Target: {}\n// ByteCode: {}\n{}", SC_UTF16ToUTF8(targetProfile.c_str()).c_str(), (mCompilerBackend == Backend::DXIL) ? "DXIL" : "SPIR-V", (mDebugShaders) ? "// --DEBUG--\n" : "");

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
		SR_ComPtr<IDxcCompiler3> dxcCompiler;
		HRESULT result = mDxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));

		SR_ComPtr<IDxcUtils> dxcUtils;
		result = mDxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));

		SR_ComPtr<IDxcBlobEncoding> sourceBlob;
		dxcUtils->CreateBlobFromPinned(shaderCode.data(), (UINT32)shaderCode.size(), 0, & sourceBlob);

		DxcBuffer sourceBuffer;
		sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
		sourceBuffer.Size = sourceBlob->GetBufferSize();
		sourceBuffer.Encoding = 0;

		SC_Array<LPCWSTR> compileArgs;
		compileArgs.Reserve(64);

		uint32 compilerFlags = SR_CompilerFlag_None;

		if (mDebugShaders)
			compilerFlags |= SR_CompilerFlag_Debug;
		if (mSkipOptimizations)
			compilerFlags |= SR_CompilerFlag_SkipOptimizations;

		SetupArgs(compileArgs, compilerFlags);

		compileArgs.Add(L"-E");
		std::wstring entryPoint = SC_UTF8ToUTF16(aArgs.mEntryPoint);
		compileArgs.Add(entryPoint.c_str());

		compileArgs.Add(L"-T");
		compileArgs.Add(targetProfile.c_str());

		//compileArgs.Add(L"-Vi");

		SC_Array<std::wstring> rawStrDefines;
		for (uint32 i = 0; i < aArgs.mDefines.Count(); ++i)
		{
			compileArgs.Add(L"-D");

			std::wstring& define = rawStrDefines.Add(SC_UTF8ToUTF16(aArgs.mDefines[i].mFirst.c_str()));
			define += L"=";
			define += SC_UTF8ToUTF16(aArgs.mDefines[i].mSecond.c_str());

			compileArgs.Add(define.c_str());
		}

		//compileArgs.Add(L"-I");
		//
		//SC_FilePath includeDir = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders";
		//std::wstring includeDirStr = SC_UTF8ToUTF16(includeDir.GetStr());
		//compileArgs.Add(includeDirStr.c_str());

		SR_ComPtr<IDxcResult> compileResults;
		SR_DxcIncludeHandler includeHandler(dxcUtils.Get());

		result = dxcCompiler->Compile(&sourceBuffer, compileArgs.GetBuffer(), compileArgs.Count(), &includeHandler, IID_PPV_ARGS(&compileResults));
		if (compileResults->HasOutput(DXC_OUT_ERRORS))
		{
			SR_ComPtr<IDxcBlobUtf8> errors;
			SR_ComPtr<IDxcBlobUtf16> errorsOutputName;
			result = compileResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), &errorsOutputName);

			if (errors && errors->GetStringLength() > 0)
			{
				SC_ERROR("Shader compilation error: {}", errors->GetStringPointer());
				return false;
			}
		}

		if (compileResults->HasOutput(DXC_OUT_OBJECT))
		{
			SR_ComPtr<IDxcBlob> shaderByteCode;
			result = compileResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderByteCode), nullptr);
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
			result = compileResults->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflectionData), nullptr);
			if (SUCCEEDED(result))
			{
				DxcBuffer reflectionBuffer;
				reflectionBuffer.Ptr = reflectionData->GetBufferPointer();
				reflectionBuffer.Size = reflectionData->GetBufferSize();
				reflectionBuffer.Encoding = 0;

#if SR_ENABLE_DX12
				SR_ComPtr<ID3D12ShaderReflection> reflection;
				dxcUtils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(&reflection));
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

		if (compileResults->HasOutput(DXC_OUT_PDB))
		{
			SR_ComPtr<IDxcBlob> pdbData;
			SR_ComPtr<IDxcBlobUtf16> outputName;
			result = compileResults->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdbData), &outputName);
			if (SUCCEEDED(result))
			{
				static constexpr const char* gShaderDebugCacheFolder = "Cache/ShaderDebug";
				SC_FilePath::CreateDirectory(gShaderDebugCacheFolder);

				std::string path = SC_FormatStr("{}/{}.shaderpdb", gShaderDebugCacheFolder, cacheEntry.GetFileName());
				std::ofstream outStream(path, std::ios::binary);
				if (outStream.is_open())
					outStream.write((const char*)pdbData->GetBufferPointer(), pdbData->GetBufferSize());
				else
				{
					SC_ERROR("Couldn't write shader debug info.");
				}
			}
		}

		return cacheEntry.WriteShaderData(aOutResult, aOutMetaData);
	}
}

void SR_DirectXShaderCompiler::SetupArgs(SC_Array<LPCWSTR>& aOutArgs, uint32 aCompilerFlags)
{
	if (aCompilerFlags & SR_CompilerFlag_Enable16BitTypes)
		aOutArgs.Add(L"-enable-16bit-types");
	if (aCompilerFlags & SR_CompilerFlag_EnablePayloadQualifiers)
		aOutArgs.Add(L"-enable-payload-qualifiers");
	if (aCompilerFlags & SR_CompilerFlag_IEEEStrictness)
		aOutArgs.Add(DXC_ARG_IEEE_STRICTNESS);

	if (mDebugShaders || (aCompilerFlags & SR_CompilerFlag_Debug))
		aOutArgs.Add(DXC_ARG_DEBUG);

	if (mSkipOptimizations || (aCompilerFlags & SR_CompilerFlag_SkipOptimizations))
		aOutArgs.Add(DXC_ARG_SKIP_OPTIMIZATIONS);
	else
	{
		if (mShaderOptimizationLevel == 2)
			aOutArgs.Add(DXC_ARG_OPTIMIZATION_LEVEL2);
		else if (mShaderOptimizationLevel == 1)
			aOutArgs.Add(DXC_ARG_OPTIMIZATION_LEVEL1);
		else if (mShaderOptimizationLevel == 0)
			aOutArgs.Add(DXC_ARG_OPTIMIZATION_LEVEL0);
		else
			aOutArgs.Add(DXC_ARG_OPTIMIZATION_LEVEL3);
	}

	aOutArgs.Add(L"-D");
	if (mCompilerBackend == Backend::SPIRV)
		aOutArgs.Add(L"IS_SPIRV=1");
	else
		aOutArgs.Add(L"IS_DXIL=1");

	if (mCompilerBackend == Backend::SPIRV)
	{
		aOutArgs.Add(L"-spirv");
		aOutArgs.Add(L"-fvk-use-dx-layout");
		aOutArgs.Add(L"-fvk-use-dx-position-w");
	}

	aOutArgs.Add(L"-HV 2021"); // TEMPORARY UNTIL HLSL 2021 BECOMES DEFAULT
	aOutArgs.Add(L"-Qstrip_debug");
	aOutArgs.Add(L"-Qstrip_rootsignature");
	aOutArgs.Add(L"-Qstrip_reflect");
	aOutArgs.Add(L"-flegacy-macro-expansion");
	aOutArgs.Add(DXC_ARG_ALL_RESOURCES_BOUND);
	aOutArgs.Add(DXC_ARG_RESOURCES_MAY_ALIAS);
}
