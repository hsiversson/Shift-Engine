#pragma once

struct alignas(16) SGfx_ShadowConstants
{
	SC_Matrix mCSMWorldToClip[4];
	SC_Vector4 mCSMSplitPoints;
	SC_IntVector4 mCSMDescriptorIndices;
	SC_Vector2 mCSMResolutionAndInv;
	SC_Vector2 __pad;
};