#pragma once

struct SGfx_MeshShadingDrawInfoStruct
{
	uint32 mVertexBufferDescriptorIndex;
	uint32 mMeshletBufferDescriptorIndex;
	uint32 mPrimitiveIndexBufferDescriptorIndex;
	uint32 mVertexIndexBufferDescriptorIndex;

	uint32 mMaterialIndex;
	uint32 mBaseInstanceDataOffset;
	uint32 mInstanceDataByteSize;
	uint32 mNumInstances;

	uint32 mInstanceOffset;
	uint32 mNumMeshlets;
	uint32 pad[2];
};

struct SGfx_VertexShadingDrawInfoStruct
{
	uint32 mMaterialIndex;
	uint32 mBaseInstanceDataOffset;
	uint32 mInstanceDataByteSize;
	uint32 mInstanceOffset;
};