#pragma once

struct SGfx_MeshShadingDrawInfoStruct
{
	uint32 mVertexBufferDescriptorIndex;
	uint32 mMeshletBufferDescriptorIndex;
	uint32 mPrimitiveIndexBufferDescriptorIndex;
	uint32 mVertexIndexBufferDescriptorIndex;

	uint32 mBaseInstanceDataOffset;
	uint32 mInstanceDataByteSize;
	uint32 mNumInstances;
	uint32 mInstanceOffset;

	uint32 mNumMeshlets;
	uint32 pad[3];
};

struct SGfx_VertexShadingDrawInfoStruct
{
	uint32 mBaseInstanceDataOffset;
	uint32 mInstanceDataByteSize;
	uint32 mInstanceOffset;
	uint32 pad;
};