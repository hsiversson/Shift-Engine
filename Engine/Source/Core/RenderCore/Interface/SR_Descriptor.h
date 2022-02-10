#pragma once

struct SR_Descriptor
{
	SR_Descriptor() : mDescriptorHandleCPU(0), mDescriptorHandleGPU(0), mHeapIndex(gInvalidIndex) {}

	static constexpr uint32 gInvalidIndex = uint32(-1);

	uint64 mDescriptorHandleCPU;
	uint64 mDescriptorHandleGPU;
	uint32 mHeapIndex;
};