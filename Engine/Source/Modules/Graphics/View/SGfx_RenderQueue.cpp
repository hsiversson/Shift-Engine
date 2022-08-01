#include "SGfx_RenderQueue.h"
#include "SGfx_ViewData.h"
#include "Graphics/Renderer/SGfx_DrawInfo.h"

SGfx_RenderQueue::SGfx_RenderQueue(const SortType& aSortType)
	: mSortType(aSortType)
	, mIsPrepared(false)
{

}

SGfx_RenderQueue::~SGfx_RenderQueue()
{

}

void SGfx_RenderQueue::Prepare(SGfx_ViewData& aPrepareData)
{
	Sort();

	SC_ASSERT(!mIsPrepared);

	SGfx_RenderQueueItem* items = mItems.GetBuffer();
	uint32 numItemsRemaining = mItems.Count();
	if (!numItemsRemaining)
		return;

	SC_Array<InstanceData> instanceDatas;
	while (numItemsRemaining)
	{
		SGfx_RenderQueueItem& firstItem = *items;
		uint32 numInstances = 1;

		SR_RasterizerState* rasterState = firstItem.mRasterizerState;
		SR_BlendState* blendState = firstItem.mBlendState;

		bool isItemValid = (firstItem.mVertexBuffer && firstItem.mIndexBuffer);
#if SR_ENABLE_MESH_SHADERS
		isItemValid = (firstItem.mUsingMeshlets) ? firstItem.mMeshletData : isItemValid;
#endif //SR_ENABLE_MESH_SHADERS

		if (isItemValid)
		{
			InstanceData& firstInstanceData = instanceDatas.Add();
			firstInstanceData.mTransform = firstItem.mTransform;
			firstInstanceData.mPrevTransform = firstItem.mPrevTransform;
			firstInstanceData.mMaterialIndex = firstItem.mMaterialIndex;

			for (uint32 i = 1; i < numItemsRemaining; ++i)
			{
				SGfx_RenderQueueItem& nextItem = items[i];
				if (nextItem.mRasterizerState != rasterState ||
					nextItem.mBlendState != blendState ||
					numInstances != SC_UINT16_MAX)
					break;

#if SR_ENABLE_MESH_SHADERS
				if (firstItem.mUsingMeshlets &&	(nextItem.mMeshletData != firstItem.mMeshletData))
					break;
				else
#endif //SR_ENABLE_MESH_SHADERS
				{
					if ((nextItem.mVertexBuffer != firstItem.mVertexBuffer) || (nextItem.mIndexBuffer != firstItem.mIndexBuffer))
						break;
				}

				InstanceData& instanceData = instanceDatas.Add();
				instanceData.mTransform = firstItem.mTransform;
				instanceData.mPrevTransform = firstItem.mPrevTransform;
				instanceData.mMaterialIndex = firstItem.mMaterialIndex;
				++numInstances;
			}
		}

		aPrepareData.mInstanceData->Add(firstItem.mBaseInstanceDataOffset, instanceDatas.GetByteSize() / sizeof(SC_Vector4f), reinterpret_cast<const SC_Vector4*>(instanceDatas.GetBuffer()));

		firstItem.mNumInstances = uint16(numInstances);
		items += numInstances;
		numItemsRemaining -= numInstances;
		instanceDatas.RemoveAll();
	}

	mIsPrepared = true;
}

void SGfx_RenderQueue::Render(SR_CommandList* aCmdList) const
{
	uint32 numItemsRemaining = mItems.Count();
	if (!numItemsRemaining)
		return;

	SC_ASSERT(mIsPrepared);

	// Create temp instance buffer data

	const SGfx_RenderQueueItem* items = mItems.GetBuffer();
	while (numItemsRemaining)
	{
		const SGfx_RenderQueueItem& item = *items;
		uint32 numInstances = item.mNumInstances;

#if SR_ENABLE_MESH_SHADERS
		if (item.mUsingMeshlets)
		{
			if (item.mMeshletData)
			{
				static constexpr uint32 gMaxGroupDispatchCount = 65536;
				float groupsPerInstance = (float)item.mMeshletData.mMeshletBuffer->GetProperties().mElementCount;
				uint32 maxInstancePerBatch = static_cast<uint32>(float(gMaxGroupDispatchCount) / groupsPerInstance);
				uint32 dispatchCount = (numInstances + maxInstancePerBatch - 1) / maxInstancePerBatch;

				for (uint32 i = 0; i < dispatchCount; ++i)
				{
					uint32 instanceOffset = maxInstancePerBatch * i;
					uint32 instanceCount = SC_Min(numInstances - instanceOffset, maxInstancePerBatch);

					SGfx_MeshShadingDrawInfoStruct drawInfo;
					drawInfo.mVertexBufferDescriptorIndex = item.mMeshletData.mVertexBuffer->GetDescriptorHeapIndex();
					drawInfo.mMeshletBufferDescriptorIndex = item.mMeshletData.mMeshletBuffer->GetDescriptorHeapIndex();
					drawInfo.mVertexIndexBufferDescriptorIndex = item.mMeshletData.mVertexIndexBuffer->GetDescriptorHeapIndex();
					drawInfo.mPrimitiveIndexBufferDescriptorIndex = item.mMeshletData.mPrimitiveIndexBuffer->GetDescriptorHeapIndex();
					drawInfo.mMaterialIndex = item.mMaterialIndex;
					drawInfo.mBaseInstanceDataOffset = item.mBaseInstanceDataOffset;
					drawInfo.mInstanceDataByteSize = sizeof(InstanceData);
					drawInfo.mNumInstances = instanceCount;
					drawInfo.mInstanceOffset = instanceOffset;
					drawInfo.mNumMeshlets = item.mMeshletData.mMeshletBuffer->GetProperties().mElementCount;

					uint64 cbOffset = 0;
					SR_BufferResource* cb = aCmdList->GetBufferResource(cbOffset, SR_BufferBindFlag_ConstantBuffer, sizeof(drawInfo), &drawInfo, 1);
					aCmdList->SetRootConstantBuffer(cb, cbOffset, 0);

					aCmdList->SetShaderState(item.mShader);
					aCmdList->SetPrimitiveTopology(SR_PrimitiveTopology::TriangleList);

					uint32 groupCount = static_cast<uint32>(ceilf(groupsPerInstance * instanceCount));
					aCmdList->DispatchMesh(groupCount, 1, 1);
				}
			}
		}
		else
#endif //SR_ENABLE_MESH_SHADERS
		{
			SR_BufferResource* vertexBuffer = item.mVertexBuffer;
			SR_BufferResource* indexBuffer = item.mIndexBuffer;
			if (vertexBuffer && indexBuffer)
			{
				SGfx_VertexShadingDrawInfoStruct constants;
				constants.mBaseInstanceDataOffset = item.mBaseInstanceDataOffset;
				constants.mInstanceDataByteSize = sizeof(InstanceData);
				constants.mMaterialIndex = item.mMaterialIndex;
				constants.mInstanceOffset = 0;

				uint64 cbOffset = 0;
				SR_BufferResource* cb = aCmdList->GetBufferResource(cbOffset, SR_BufferBindFlag_ConstantBuffer, sizeof(constants), &constants, 1);
				aCmdList->SetRootConstantBuffer(cb, cbOffset, 0);

				aCmdList->SetVertexBuffer(vertexBuffer);
				aCmdList->SetIndexBuffer(indexBuffer);
				aCmdList->SetShaderState(item.mShader);
				aCmdList->SetPrimitiveTopology(SR_PrimitiveTopology::TriangleList);
				aCmdList->DrawIndexedInstanced(indexBuffer->GetProperties().mElementCount, numInstances);
			}
		}

		items += numInstances;
		numItemsRemaining -= numInstances;
	}
}

void SGfx_RenderQueue::Clear()
{
	mItems.RemoveAll();
	mIsPrepared = false;
}

SGfx_RenderQueueItem& SGfx_RenderQueue::AddItem()
{
	return mItems.Add();
}

SGfx_RenderQueueItem& SGfx_RenderQueue::AddItem(const SGfx_RenderQueueItem& aItem)
{
	return mItems.Add(aItem);
}

uint32 SGfx_RenderQueue::NumItems() const
{
	return mItems.Count();
}

bool SGfx_RenderQueue::IsEmpty() const
{
	return mItems.IsEmpty();
}

void SGfx_RenderQueue::Sort()
{
	switch (mSortType)
	{
	case SortType::ByState:
		Sort_ByState();
		break;
	case SortType::FarFirst:
		Sort_FarFirst();
		break;
	case SortType::NearFirst:
		Sort_NearFirst();
		break;
	}
}

void SGfx_RenderQueue::Sort_ByState()
{
	auto Comparison = [](const SGfx_RenderQueueItem& aLeft, const SGfx_RenderQueueItem& aRight)
	{
		if (aLeft.mUsingMeshlets != aRight.mUsingMeshlets)
			return aLeft.mUsingMeshlets < aRight.mUsingMeshlets;

		if (aLeft.mRasterizerState != aRight.mRasterizerState)
			return aLeft.mRasterizerState < aRight.mRasterizerState;

		if (aLeft.mBlendState != aRight.mBlendState)
			return aLeft.mBlendState < aRight.mBlendState;

		if (aLeft.mShader != aRight.mShader)
			return aLeft.mShader < aRight.mShader;

		if (aLeft.mVertexBuffer != aRight.mVertexBuffer)
			return aLeft.mVertexBuffer < aRight.mVertexBuffer;

		if (aLeft.mIndexBuffer != aRight.mIndexBuffer)
			return aLeft.mIndexBuffer < aRight.mIndexBuffer;

		return aLeft.mSortDistance < aRight.mSortDistance;
	};

	std::sort(mItems.begin(), mItems.end(), Comparison);
}

void SGfx_RenderQueue::Sort_FarFirst()
{
	auto Comparison = [](const SGfx_RenderQueueItem& aLeft, const SGfx_RenderQueueItem& aRight)
	{
		if (aLeft.mSortDistance != aRight.mSortDistance)
			return aLeft.mSortDistance > aRight.mSortDistance;

		if (aLeft.mUsingMeshlets != aRight.mUsingMeshlets)
			return aLeft.mUsingMeshlets < aRight.mUsingMeshlets;

		if (aLeft.mRasterizerState != aRight.mRasterizerState)
			return aLeft.mRasterizerState < aRight.mRasterizerState;

		if (aLeft.mBlendState != aRight.mBlendState)
			return aLeft.mBlendState < aRight.mBlendState;

		if (aLeft.mShader != aRight.mShader)
			return aLeft.mShader < aRight.mShader;

		if (aLeft.mVertexBuffer != aRight.mVertexBuffer)
			return aLeft.mVertexBuffer < aRight.mVertexBuffer;

		return aLeft.mIndexBuffer < aRight.mIndexBuffer;
	};

	std::sort(mItems.begin(), mItems.end(), Comparison);
}

void SGfx_RenderQueue::Sort_NearFirst()
{
	auto Comparison = [](const SGfx_RenderQueueItem& aLeft, const SGfx_RenderQueueItem& aRight)
	{
		if (aLeft.mSortDistance != aRight.mSortDistance)
			return aLeft.mSortDistance < aRight.mSortDistance;

		if (aLeft.mUsingMeshlets != aRight.mUsingMeshlets)
			return aLeft.mUsingMeshlets < aRight.mUsingMeshlets;

		if (aLeft.mRasterizerState != aRight.mRasterizerState)
			return aLeft.mRasterizerState < aRight.mRasterizerState;

		if (aLeft.mBlendState != aRight.mBlendState)
			return aLeft.mBlendState < aRight.mBlendState;

		if (aLeft.mShader != aRight.mShader)
			return aLeft.mShader < aRight.mShader;

		if (aLeft.mVertexBuffer != aRight.mVertexBuffer)
			return aLeft.mVertexBuffer < aRight.mVertexBuffer;

		return aLeft.mIndexBuffer < aRight.mIndexBuffer;
	};

	std::sort(mItems.begin(), mItems.end(), Comparison);
}
