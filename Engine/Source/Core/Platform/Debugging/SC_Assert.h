#pragma once
#include "Platform/Platform/SC_PlatformDefines.h"
#include <cassert>
#undef assert

#if !IS_RELEASE_BUILD

#if IS_MSVC_COMPILER
#define SC_DEBUG_BREAK()	__debugbreak()
#else
#error Not supported on this platform yet
#endif

bool SC_Assert(const char* aFile, int aLine, const char* aString);
bool SC_Assert(const char* aFile, int aLine, const char* aString, const char* aMessageFormatString, ...);

#define SC_FORCE_ASSERT(cond, ...) \
	do { \
		if(!cond) \
		{ \
			if(SC_Assert(__FILE__, __LINE__, (#cond), ##__VA_ARGS__)) \
				SC_DEBUG_BREAK(); \
		} \
	} while(0)


#define SC_ASSERT(cond, ...) SC_FORCE_ASSERT((cond), ##__VA_ARGS__)

#define assert(...) SC_ASSERT(__VA_ARGS__)

#else // !IS_RELEASE_BUILD

#define SC_ASSERT(cond, ...)
#define assert(...)

#endif // !IS_RELEASE_BUILD