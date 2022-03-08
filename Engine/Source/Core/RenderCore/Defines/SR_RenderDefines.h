#pragma once

#include "Platform/Platform/SC_PlatformDefines.h"

#if IS_DEBUG_BUILD
	#define SR_ENABLE_DEBUG_NAMES		(1)
#endif


#if IS_WINDOWS_PLATFORM
	#define ENABLE_DX12					(1)
	#define ENABLE_VULKAN				(0)
	#define ENABLE_RAYTRACING			(1)
	#define ENABLE_MESH_SHADERS			(1)
	#define ENABLE_VRS					(1)
	#define ENABLE_PIX					(1)

	#if IS_DEBUG_BUILD
		#define ENABLE_RENDERDOC_API	(0)
	#endif

#endif

#define ENABLE_GPU_PROFILER				(ENABLE_PROFILER)

#ifndef SR_ENABLE_DEBUG_NAMES
	#define SR_ENABLE_DEBUG_NAMES		(0)
#endif

#ifndef ENABLE_DX12
	#define ENABLE_DX12					(0)
#endif

#ifndef ENABLE_DX12
	#define ENABLE_VULKAN				(0)
#endif

#ifndef ENABLE_RAYTRACING
	#define ENABLE_RAYTRACING			(0)
#endif

#ifndef ENABLE_MESH_SHADERS
	#define ENABLE_MESH_SHADERS			(0)
#endif

#ifndef ENABLE_VRS
	#define ENABLE_VRS					(0)
#endif

#ifndef ENABLE_RENDERDOC_API
	#define ENABLE_RENDERDOC_API		(0)
#endif

#define SR_MAX_RENDER_TARGETS			(8)