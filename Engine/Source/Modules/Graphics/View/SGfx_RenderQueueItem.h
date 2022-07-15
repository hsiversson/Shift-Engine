#pragma once

class SR_ShaderState;
class SR_BufferResource;

struct SR_RasterizerState;
struct SR_BlendState;

struct SGfx_RenderQueueItemMeshletData
{
	operator bool() const
	{
		return (mVertexBuffer && mMeshletBuffer && mVertexIndexBuffer && mPrimitiveIndexBuffer);
	}

	SR_Buffer* mVertexBuffer;
	SR_Buffer* mMeshletBuffer;
	SR_Buffer* mVertexIndexBuffer;
	SR_Buffer* mPrimitiveIndexBuffer;
};

struct SGfx_RenderQueueItem
{
	SGfx_RenderQueueItem()
	{
		SC_ZeroMemory(this, sizeof(*this));
	}

	union 
	{
		struct
		{
			SR_BufferResource* mVertexBuffer;
			SR_BufferResource* mIndexBuffer;
		};
		SGfx_RenderQueueItemMeshletData mMeshletData;
	};

	SC_Matrix mTransform;
	SC_Matrix mPrevTransform;

	uint32 mMaterialIndex;

	SR_RasterizerState* mRasterizerState;
	SR_BlendState* mBlendState;

	SR_ShaderState* mShader;

	float mSortDistance;
	uint32 mBaseInstanceDataOffset;
	uint16 mNumInstances;

	bool mUsingMeshlets;
};