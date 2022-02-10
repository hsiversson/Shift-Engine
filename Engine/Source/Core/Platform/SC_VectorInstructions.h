#pragma once
#include "SC_PlatformDefines.h"

#define SC_VECTOR_INSTRUCTIONS_HEADER

#if ENABLE_SIMD_INSTRUCTIONS
#include "SC_VectorInstructions_SSE.h"
#else
#include "SC_VectorInstructions_Generic.h"
#endif