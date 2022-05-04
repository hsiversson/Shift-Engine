#include "SC_System.h"
#include "Platform/Platform/SC_Platform.h"

#if IS_WINDOWS_PLATFORM
	#include <Psapi.h>
	#include <sysinfoapi.h>
#endif

namespace SC_System
{
	SC_SizeT GetPhysicalMemoryAvailable()
	{
#if IS_WINDOWS_PLATFORM
		MEMORYSTATUSEX status = {};
		status.dwLength = sizeof(status);
		if (GlobalMemoryStatusEx(&status))
			return status.ullTotalPhys;
#endif
		return 0;
	}

	SC_SizeT GetPhysicalMemoryUsed()
	{
#if IS_WINDOWS_PLATFORM
		PROCESS_MEMORY_COUNTERS_EX memoryCounter = {};
		if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&memoryCounter, sizeof(memoryCounter)))
		{
			return memoryCounter.WorkingSetSize;
		}
#endif
		return 0;
	}

	SC_SizeT GetVirtualMemoryAvailable()
	{
#if IS_WINDOWS_PLATFORM
		MEMORYSTATUSEX status = {};
		status.dwLength = sizeof(status);
		if (GlobalMemoryStatusEx(&status))
			return status.ullTotalPageFile;
#endif
		return 0;
	}

	SC_SizeT GetVirtualMemoryUsed()
	{
#if IS_WINDOWS_PLATFORM
		PROCESS_MEMORY_COUNTERS_EX memoryCounter = {};
		if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&memoryCounter, sizeof(memoryCounter)))
		{
			return memoryCounter.PrivateUsage;
		}
#endif
		return 0;
	}
}