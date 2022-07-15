#include "SC_Platform_PC.h"

#if IS_PC_PLATFORM

#if IS_WINDOWS_PLATFORM
	#include <Psapi.h>
	#include <sysinfoapi.h>

	#if IS_X86_PLATFORM
		#include <intrin.h>
		#pragma intrinsic(__cpuid)
	#endif //IS_X86_PLATFORM
#endif //IS_WINDOWS_PLATFORM

namespace SC_Platform
{
    CpuVendor GetCpuVendor()
    {
		static CpuVendor gVendor = CpuVendor::Unknown;
		if (gVendor == CpuVendor::Unknown)
		{
#if IS_WINDOWS_PLATFORM
			HKEY hKey;
			if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
			{
				WCHAR vendorName[128];
				DWORD bufSize = sizeof(vendorName);
				if (RegQueryValueEx(hKey, TEXT("VendorIdentifier"), nullptr, nullptr, (LPBYTE)vendorName, &bufSize) == ERROR_SUCCESS)
				{
					if (wcscmp(TEXT("GenuineIntel"), vendorName) == 0)
						gVendor = CpuVendor::Intel;
					else if (wcscmp(TEXT("AuthenticAMD"), vendorName) == 0)
						gVendor = CpuVendor::AMD;
				}

				RegCloseKey(hKey);
			}
#endif
		}

        return gVendor;
	}

	uint32 GetCpuFrequencyMHz()
	{
		static uint32 gFrequency = SC_UINT32_MAX;
		if (gFrequency == SC_UINT32_MAX)
		{
#if IS_WINDOWS_PLATFORM
			HKEY hKey;
			if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
			{
				DWORD bufSize = sizeof(gFrequency);
				if (RegQueryValueExA(hKey, "~MHz", nullptr, nullptr, (LPBYTE)&gFrequency, &bufSize) != ERROR_SUCCESS)
				{
					gFrequency = 0; // Default to 0 on Error.
				}

				RegCloseKey(hKey);
			}
#endif
		}

		return gFrequency;
	}

#if IS_X86_PLATFORM
    uint32 GetCpuidFamily()
    {
        static uint32 gFamily = SC_UINT32_MAX;
		if (gFamily == SC_UINT32_MAX)
		{
			int data[4];
			__cpuid(data, 1);
			int family = ((data[0] >> 8) & 0x0F);
			int extendedFamily = (data[0] >> 20) & 0xFF;
			gFamily = (family != 0x0F) ? family : (extendedFamily + family);
		}

        return gFamily;
    }

    uint32 GetCpuidModel(const CpuVendor& aVendor, int aCpuFamily)
	{
		static uint32 gModel = SC_UINT32_MAX;
		if (gModel == SC_UINT32_MAX)
		{
			int data[4];
			__cpuid(data, 1);
			int model = ((data[0] >> 4) & 0x0F);
			int extendedModel = (data[0] >> 16) & 0x0F;

			if (aVendor == CpuVendor::Intel)
				gModel = (aCpuFamily == 0x06 || aCpuFamily == 0x0F) ? ((extendedModel << 4) + model) : model;
			else if (aVendor == CpuVendor::AMD)
				gModel = (aCpuFamily < 0x0F) ? model : ((extendedModel << 4) + model);
		}

        return gModel;
    }

    uint32 GetCpuidFlopsPerCycle()
	{
		static uint32 gFlopsPerCycle = SC_UINT32_MAX;
		if (gFlopsPerCycle == SC_UINT32_MAX)
		{
			const CpuVendor cpuVendor = GetCpuVendor();
			int cpuFamily = GetCpuidFamily();
			int cpuModel = GetCpuidModel(cpuVendor, cpuFamily);
			if (cpuVendor == CpuVendor::Intel && cpuFamily >= 0x6)
			{
				if (cpuModel >= 106) // Ice Lake or higher
					return 64;
				else if (cpuModel >= 60) // Haswell or higher
					return 32;
				else if (cpuModel >= 42) // Sandy Bridge or higher
					return 16;
			}
			else if (cpuVendor == CpuVendor::AMD && cpuFamily >= 0x15)
			{
				if (cpuFamily >= 0x19) // Zen3 or higher
					return 32;
				else if (cpuFamily >= 0x17)
				{
					if (cpuModel >= 49) // Zen2 or higher
						return 32;
					else // Zen or higher
						return 16;
				}
				else
					return 8;
			}

			return 1;
		}

        return gFlopsPerCycle;
    }

	float GetCpuidGFLOPS()
	{
		float GHz = GetCpuFrequencyMHz() / 1000.f;
		uint32 flops = GetCpuidFlopsPerCycle();
		uint32 numCores = 0;

		return GHz * flops * numCores;
	}
#endif //IS_X86_PLATFORM

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
#endif