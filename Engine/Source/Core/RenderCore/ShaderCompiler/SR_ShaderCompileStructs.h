#pragma once

struct SR_ShaderCompileArgs
{
	SR_ShaderCompileArgs() : mType(SR_ShaderType::COUNT), mEntryPoint(nullptr) {}

	SC_Array<SC_Pair<std::string, std::string>> mDefines;
	SR_ShaderType mType;
	SC_FilePath mShaderFile;
	const char* mEntryPoint;
};

struct SR_ShaderByteCode
{
	SR_ShaderByteCode() : mSize(0), mByteCode(nullptr) {}
	SR_ShaderByteCode(const SR_ShaderByteCode& aOther)
		: mSize(aOther.mSize)
		, mByteCode(nullptr)
	{
		if (mSize > 0 || aOther.mByteCode == nullptr)
		{
			mByteCode = SC_MakeUnique<uint8[]>(mSize);
			SC_Memcpy(mByteCode.get(), aOther.mByteCode.get(), mSize);
		}
	}

	SR_ShaderByteCode& operator=(const SR_ShaderByteCode& aOther) 
	{
		mSize = aOther.mSize;
		if (mSize > 0 || aOther.mByteCode == nullptr)
		{
			mByteCode = SC_MakeUnique<uint8[]>(mSize);
			SC_Memcpy(mByteCode.get(), aOther.mByteCode.get(), mSize);
		}
		return *this;
	}

	uint64 mSize;
	SC_UniquePtr<uint8[]> mByteCode;
	std::string mEntryPoint;
};