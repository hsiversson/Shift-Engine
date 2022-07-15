#pragma once
#include "SC_VirtualMachineEnums_SIMD.h"

class SC_VirtualMachineContext_SIMD
{
public:
	SC_VirtualMachineContext_SIMD();

	void Prepare();
	void PrepareChunk();

	void Finish();

	uint16 DecodeU16();

	template<typename T>
	T* GetConstant(uint32 /*aIndex*/)
	{
		return nullptr;
	}

	void* GetTempRegister(uint32 /*aIndex*/)
	{
		return nullptr;
	}

	static SC_VirtualMachineContext_SIMD& Get() 
	{ 
		static thread_local SC_VirtualMachineContext_SIMD gContext;
		return gContext;
	}

public:
	uint8 const* mNextByteCodeElement;

	const uint8* const* mConstantTable;
	const uint32* mConstantTableSize;
	uint32 mConstantTableCount;

	uint32 mTempRegistersCount;

	const void* const* mExternalFunctionTable;
	void** mUserPtrTable;

	uint32 mInstanceCount;
	uint32 mInstanceCountSIMDBatch;
	uint32 mStartInstance;

	SC_Array<uint8> mTempRegisterTable;
	uint32 mTempRegisterSize;
	uint32 mTempBufferSize;

	//Rand
	//RandCounter

	uint32 mValidInstanceStartIndex;
	uint32 mValidInstanceCount;
	bool mIsValidInstanceUniform;
	bool mIsParallellExec;
};

