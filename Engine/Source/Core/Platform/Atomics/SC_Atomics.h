#pragma once

#include "Platform/Platform/SC_PlatformDefines.h"

#if IS_WINDOWS_PLATFORM
#include "SC_Atomics_Win64.h"
#else
#error Platform not supported!
#endif