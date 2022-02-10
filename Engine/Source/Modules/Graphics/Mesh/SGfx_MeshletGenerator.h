#pragma once
#include "SGfx_Meshlet.h"
#include "RenderCore/Interface/SR_VertexLayout.h"

struct SGfx_MeshletSubset
{
	uint32 mOffset;
	uint32 mCount;
};

bool SGfx_GenerateMeshlets(
	uint32 aMaxVertices,
	uint32 aMaxPrimitives,
	const uint8* aVertexData,
	const SR_VertexLayout& aVertexLayout,
	const uint32 aNumVertices,
	const uint8* aIndexData,
	bool aUse16BitIndices,
	const uint32 aNumIndices,
	SC_Array<SGfx_Meshlet>& aOutMeshlets,
	SC_Array<SGfx_PackedPrimitiveTriangle>& aOutPrimitives,
	SC_Array<uint8>& aOutVertexIndices);

bool SGfx_FlattenMeshletPrimitivesToIndexBuffer(
	const SC_Array<SGfx_Meshlet>& aMeshlets,
	const SC_Array<SGfx_PackedPrimitiveTriangle>& aPrimitives,
	const SC_Array<uint8>& aUniqueVertexIndices,
	const uint32 aVertexIndicesStride,
	SC_Array<uint8>& aOutIndexData,
	uint32& aOutIndexStride
);