#pragma once

// Platform
#include "Platform/Platform/SC_Platform.h"
#include "Platform/Misc/SC_CppRuntimeIncludes.h"
#include "Platform/System/SC_System.h"
#include "Platform/Atomics/SC_Atomics.h"
#include "Platform/Time/SC_Time.h"
#include "Platform/Async/SC_Mutex.h"
#include "Platform/Async/SC_Thread.h"

// Types
#include "EngineTypes/SC_TypesInclude.h"

// IO
#include "InputOutput/File/SC_File.h"
#include "InputOutput/File/SC_EnginePaths.h"
#include "InputOutput/Json/SC_Json.h"

// Common
#include "Common/Logging/SC_Logger.h"
#include "Common/ThreadPool/SC_ThreadPool.h"
#include "Common/Configuration/SC_CommandLine.h"
#include "Common/Console/SC_Console.h"
#include "Common/Profiling/SC_Profiler.h"