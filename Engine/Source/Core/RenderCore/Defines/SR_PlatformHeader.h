#pragma once

#include "SR_RenderDefines.h"

#if SR_ENABLE_DX12
#include "RenderCore/DX12/SR_PlatformHeader_DX12.h"
#include "RenderCore/DX12/SR_Convert_DX12.h"
#endif

#if SR_ENABLE_VULKAN
#include "RenderCore/Vulkan/SR_PlatformHeader_Vk.h"
#include "RenderCore/Vulkan/SR_Convert_Vk.h"
#endif