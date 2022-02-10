#pragma once

struct SGfx_MeshShadingDrawInfoStruct
{
	SC_Matrix mTransform;
	SC_Matrix mPrevTransform;
	uint32 mVertexBufferDescriptorIndex;
	uint32 mMeshletBufferDescriptorIndex;
	uint32 mPrimitiveIndexBufferDescriptorIndex;
	uint32 mVertexIndexBufferDescriptorIndex;
	uint32 mMaterialIndex;
	uint32 aoTex;
};

struct SGfx_VertexShadingDrawInfoStruct
{
	SC_Matrix mTransform;
	SC_Matrix mPrevTransform;
	uint32 mVertexBufferDescriptorIndex;
	uint32 mIndexBufferDescriptorIndex;
	uint32 mMaterialIndex;
};