#pragma once

#if IS_WINDOWS_PLATFORM
#pragma push_macro("assert")
#pragma push_macro("NOMINMAX")

#ifdef assert
#undef assert
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#pragma pop_macro("assert")
#pragma pop_macro("NOMINMAX")
#endif

#if IS_PC_PLATFORM
namespace SC_Platform
{
	enum class CpuVendor
	{
		Unknown,
		Intel,
		AMD
	};

	CpuVendor GetCpuVendor();
	uint32 GetCpuFrequencyMHz();

#if IS_X86_PLATFORM
	uint32 GetCpuidFamily();
	uint32 GetCpuidModel(const CpuVendor& aVendor, int aCpuFamily);
	uint32 GetCpuidFlopsPerCycle();
	float GetCpuidGFLOPS();
#endif

	SC_SizeT GetPhysicalMemoryAvailable();
	SC_SizeT GetPhysicalMemoryUsed();
	SC_SizeT GetVirtualMemoryAvailable();
	SC_SizeT GetVirtualMemoryUsed();
}
#endif