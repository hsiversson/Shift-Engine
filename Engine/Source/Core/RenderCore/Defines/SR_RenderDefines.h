#pragma once

#include "Platform/Platform/SC_PlatformDefines.h"

#if IS_DEBUG_BUILD
	#define SR_ENABLE_DEBUG_NAMES			(1)
#endif

#if IS_WINDOWS_PLATFORM
	#define SR_ENABLE_DX12					(1)
	#define SR_ENABLE_DIRECTSTORAGE			(0)
	#define SR_ENABLE_VULKAN				(0)
	#define SR_ENABLE_RAYTRACING			(1)
	#define SR_ENABLE_MESH_SHADERS			(1)
	#define SR_ENABLE_VRS					(1)

	#define SR_ENABLE_PIX					(1)
	#define SR_ENABLE_DRED					(1)
	#define SR_ENABLE_RENDERDOC_API			(0)		// Deprecated
	#define SR_ENABLE_NVIDIA_AFTERMATH		(0)		// Experimental

#endif

#define SR_MAX_RENDER_TARGETS				(8)

#define SR_ENABLE_GPU_PROFILER				(ENABLE_PROFILER)

#ifndef SR_ENABLE_DEBUG_NAMES
	#define SR_ENABLE_DEBUG_NAMES			(0)
#endif

#ifndef SR_ENABLE_DX12
	#define SR_ENABLE_DX12					(0)
#endif

#ifndef SR_ENABLE_DIRECTSTORAGE
	#define SR_ENABLE_DIRECTSTORAGE			(0)
#endif

#ifndef SR_ENABLE_VULKAN
	#define SR_ENABLE_VULKAN				(0)
#endif

#ifndef SR_ENABLE_RAYTRACING
	#define SR_ENABLE_RAYTRACING			(0)
#endif

#ifndef SR_ENABLE_MESH_SHADERS
	#define SR_ENABLE_MESH_SHADERS			(0)
#endif

#ifndef SR_ENABLE_VRS
	#define SR_ENABLE_VRS					(0)
#endif

#ifndef SR_ENABLE_PIX
	#define SR_ENABLE_PIX					(0)
#endif

#ifndef SR_ENABLE_DRED
	#define SR_ENABLE_DRED					(0)
#endif

#ifndef SR_ENABLE_RENDERDOC_API
	#define SR_ENABLE_RENDERDOC_API			(0)
#endif

#ifndef SR_ENABLE_NVIDIA_AFTERMATH
	#define SR_ENABLE_NVIDIA_AFTERMATH		(0)
#endif