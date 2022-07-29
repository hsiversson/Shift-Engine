#pragma once

// Configuration 
#ifdef NDEBUG
	#define IS_OPTIMISED_BUILD			(1)
#else
	#define IS_DEBUG_BUILD				(1)
#endif

// Platform
#if defined(_WIN32)
	#if !defined(_WIN64)
		#error "32-bit builds are not supported!"
	#endif

	#define IS_WINDOWS_PLATFORM			(1)
	#define IS_PC_PLATFORM				(1)
	#define IS_64BIT					(1)
	#define IS_X86_PLATFORM				(1)
	#define ENABLE_SIMD_INSTRUCTIONS	(1)

#elif defined(__APPLE__) || defined(__MACH__)
	#if (TARGET_IPHONE_SIMULATOR == 1)
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_PHONE == 1
		#define IS_IOS_PLATFORM			(1)
	#elif TARGET_OS_MAC == 1
		#define IS_MAC_PLATFORM			(1)
	#endif
#elif defined(__ANDROID__)
	#define IS_ANDROID_PLATFORM			(1)
#elif defined(__linux__)
	#define IS_LINUX_PLATFORM			(1)
#endif

#if IS_WINDOWS_PLATFORM
	#if ENABLE_EDITOR				
		#define IS_EDITOR_BUILD			(1)
	#endif
#endif

// Compiler
#if _MSC_VER
#	define IS_MSVC_COMPILER				(1)
#else
#	define IS_MSVC_COMPILER				(0)
#endif

#ifdef __clang__
#   define IS_CLANG_COMPILER			(1)
#else
#   define IS_CLANG_COMPILER			(0)
#endif

#ifdef __GNUC__
#   define IS_GCC_COMPILER				(1)
#else
#   define IS_GCC_COMPILER				(0)
#endif

// RTTI Support
#if defined( _CPPRTTI ) || defined( __GNUG__ )
	#define ENABLE_RTTI					(1)
#else
	#define ENABLE_RTTI					(0)
#endif

#if !IS_MASTER_BUILD
	#define ENABLE_PROFILER				(0)
	#define ENABLE_LOGGING				(1)
	#define ENABLE_CONSOLE				(1)
#else
	#define ENABLE_PROFILER				(0)
	#define ENABLE_LOGGING				(0)
	#define ENABLE_CONSOLE				(0)
#endif

// Make sure we always have defines even if not enabled
#ifndef IS_OPTIMISED_BUILD
	#define IS_OPTIMISED_BUILD			(0)
#endif

#ifndef IS_DEBUG_BUILD
	#define IS_DEBUG_BUILD				(0)
#endif

#ifndef IS_WINDOWS_PLATFORM
	#define IS_WINDOWS_PLATFORM			(0)
#endif

#ifndef IS_PC_PLATFORM
	#define IS_PC_PLATFORM				(0)
#endif

#ifndef IS_64BIT
	#define IS_64BIT					(0)
#endif

#ifndef IS_X86_PLATFORM
	#define IS_X86_PLATFORM				(0)
#endif

#ifndef ENABLE_SIMD_INSTRUCTIONS
	#define ENABLE_SIMD_INSTRUCTIONS	(0)
#endif

#ifndef IS_EDITOR_BUILD
	#define IS_EDITOR_BUILD				(0)
#endif

#ifndef IS_IOS_PLATFORM					
	#define IS_IOS_PLATFORM				(0)
#endif

#ifndef IS_MAC_PLATFORM					
	#define IS_MAC_PLATFORM				(0)
#endif

#ifndef IS_ANDROID_PLATFORM					
	#define IS_ANDROID_PLATFORM			(0)
#endif

#ifndef IS_LINUX_PLATFORM					
	#define IS_LINUX_PLATFORM			(0)
#endif