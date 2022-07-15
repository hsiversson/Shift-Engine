#pragma once
#include <cstddef>
#include <cstdint>
#include <cfloat>

using int8  = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using uint8  = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using SC_SizeT		  = std::size_t;
using SC_PtrDiff	  = std::ptrdiff_t;
using SC_IntPtr		  = std::intptr_t;
using SC_MaxAlignType = std::max_align_t;
using SC_Nullptr	  = std::nullptr_t;

static constexpr int8  SC_INT8_MIN  = INT8_MIN;
static constexpr int8  SC_INT8_MAX  = INT8_MAX;
static constexpr uint8 SC_UINT8_MAX = UINT8_MAX;

static constexpr int16  SC_INT16_MIN  = INT16_MIN;
static constexpr int16  SC_INT16_MAX  = INT16_MAX;
static constexpr uint16 SC_UINT16_MAX = UINT16_MAX;

static constexpr int32  SC_INT32_MIN  = INT32_MIN;
static constexpr int32  SC_INT32_MAX  = INT32_MAX;
static constexpr uint32 SC_UINT32_MAX = UINT32_MAX;

static constexpr int64  SC_INT64_MIN  = INT64_MIN;
static constexpr int64  SC_INT64_MAX  = INT64_MAX;
static constexpr uint64 SC_UINT64_MAX = UINT64_MAX;

static constexpr float SC_FLT_MAX    = FLT_MAX;
static constexpr float SC_FLT_MIN    = FLT_MIN;
static constexpr float SC_FLT_LOWEST = -SC_FLT_MAX;

#define KB(Num) ((Num) * 1024)
#define MB(Num) ((Num) * (1024 * 1024))
#define GB(Num) ((Num) * (1024*1024*1024ull))

#define BYTE_TO_KB(Num) ((Num) / KB(1))
#define BYTE_TO_MB(Num) ((Num) / MB(1))
#define BYTE_TO_GB(Num) ((Num) / GB(1))