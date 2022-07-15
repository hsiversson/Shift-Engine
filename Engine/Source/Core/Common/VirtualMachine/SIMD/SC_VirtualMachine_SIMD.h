#pragma once
#include "SC_VirtualMachineEnums_SIMD.h"

class SC_VirtualMachine_SIMD
{
public:

	struct ExecuteArgs
	{
		uint8 const* mByteCode;
		uint8 const* mOptimizedByteCode;

		uint32 mTempRegisterCount;
		uint32 mConstantTableCount;

		const uint8* const* mConstantTable;
		const uint32* mConstantTableSizes;

		const void* const* mExternalFunctionTable;
		void** mUserPtrTable;

		uint32 mInstanceCount;
		bool mAllowParallellExecution;
	};
	static void Execute(ExecuteArgs& aArgs);
};

