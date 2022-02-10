#pragma once

#include "SC_MemoryAllocator.h"

class SC_GlobalData
{
public:
	SC_GlobalData();

	SC_MemoryAllocator mMemoryAllocator;
};

extern SC_ALIGN(16) uint8 _gGlobalDataInstance[sizeof(SC_GlobalData)];
#define SC_GetGlobalData() ((SC_GlobalData&)_gGlobalDataInstance)

void SC_InitGlobalData();