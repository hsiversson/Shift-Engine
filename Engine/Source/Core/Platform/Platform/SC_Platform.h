#pragma once

#include "SC_PlatformDefines.h"

#if IS_MSVC_COMPILER

	#ifndef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
		#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
	#endif

	#ifndef _CRT_SECURE_NO_DEPRECATE
		#define _CRT_SECURE_NO_DEPRECATE
	#endif

	#pragma inline_depth(255)
	#pragma inline_recursion(on)
	#pragma auto_inline(on)

	#pragma warning(disable: 4201) // nonstandard extension used : nameless struct/union
	#pragma warning(disable: 4324) // 'struct_name' : structure was padded due to __declspec(align())
	#pragma warning(disable: 4577) // 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed.
	#pragma warning(disable: 26812) // The enum type is unscoped. Prefer 'enum class' over 'enum'.

	#define SC_FORCEINLINE			__forceinline
	#define SC_NOINLINE				__declspec(noinline)
	#define SC_OPTIMIZE_OFF			__pragma(optimize("", off))
	#define SC_OPTIMIZE_ON			__pragma(optimize("", on))
	#define SC_DEPRECATED			__declspec(deprecated)
	#define SC_DEPRECATED_MSG(aMsg)	__declspec(deprecated(aMsg))
	#define SC_RESTRICT				__restrict
	#define SC_FASTCALL				__fastcall
	#define SC_ALIGN(aValue)		__declspec(align(aValue))
	#define SC_NOVTABLE				__declspec(novtable)

#elif IS_CLANG_COMPILER

	#define SC_FORCEINLINE			inline __attribute((always_inline))
	#define SC_NOINLINE				__attribute((noinline))
	#define SC_OPTIMIZE_OFF			_Pragma("clang optimize off")
	#define SC_OPTIMIZE_ON			_Pragma("clang optimize on")
	#define SC_DEPRECATED			__attribute__((deprecated))
	#define SC_DEPRECATED_MSG(aMsg)	__attribute__((deprecated(aMsg)))
	#define SC_RESTRICT				__restrict__
	#define SC_FASTCALL				
	#define SC_ALIGN(aValue)		__attribute__((__aligned__(aValue)))
	#define SC_NOVTABLE				

#elif IS_GCC_COMPILER

	#define SC_FORCEINLINE			inline __attribute((always_inline))
	#define SC_NOINLINE				__attribute((noinline))
	#define SC_OPTIMIZE_OFF			__attribute__((optimize("O0")))
	#define SC_OPTIMIZE_ON			__attribute__((optimize("O3")))
	#define SC_DEPRECATED			__attribute__((deprecated))
	#define SC_DEPRECATED_MSG(aMsg)	__attribute__((deprecated(aMsg)))
	#define SC_RESTRICT				__restrict__
	#define SC_FASTCALL				
	#define SC_ALIGN(aValue)		__attribute__((__aligned__(aValue)))
	#define SC_NOVTABLE				


#else
	#error Compiler not supported!
#endif

#define SC_STRINGIFY2(aStr)					#aStr
#define SC_STRINGIFY(aStr)					SC_STRINGIFY2(aStr)
#define SC_UNUSED(aIdentifier)				(static_cast<void>(aIdentifier))

#define SC_CONCAT2(a, b) a ## b
#define SC_CONCAT(a, b) SC_CONCAT2(a, b)

#define SC_STATIC_ASSERT(aCond, aMsg)		static_assert( aCond, #aMsg )

#define SC_ARRAY_SIZE(aArray)				(sizeof(aArray) / sizeof(*aArray))

#define SC_OVERLOAD_NEW_DELETE_OPERATORS	(0)

#include "Platform/Types/SC_TypeDefines.h"
#include "Platform/Misc/SC_CommonFunctions.h"
#include "Platform/Misc/SC_MemoryFunctions.h"
#include "Platform/Misc/SC_Relocation.h"

#if IS_PC_PLATFORM
	#include "SC_Platform_PC.h"
#else
	#error Platform not supported yet!
#endif

#include "Platform/Debugging/SC_Assert.h"