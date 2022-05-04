#include "SGfx_RenderQueue.h"

SGfx_RenderQueue::SGfx_RenderQueue(const SortType& aSortType)
	: mSortType(aSortType)
	, mIsPrepared(false)
{

}

SGfx_RenderQueue::~SGfx_RenderQueue()
{

}

void SGfx_RenderQueue::Prepare()
{
	Sort();

	assert(!mIsPrepared);

	SGfx_RenderQueueItem* items = mItems.GetBuffer();
	uint32 numItemsRemaining = mItems.Count();
	if (!numItemsRemaining)
		return;

	while (numItemsRemaining)
	{
		SGfx_RenderQueueItem& firstItem = *items;
		uint32 batchSize = 1;
		SR_BufferResource* vertexBuffer = firstItem.mVertexBuffer;
		SR_BufferResource* indexBuffer = firstItem.mIndexBuffer;
		if (vertexBuffer && indexBuffer)
		{
			// Add instance data

			SR_RasterizerState* rasterState = firstItem.mRasterizerState;
			SR_BlendState* blendState = firstItem.mBlendState;

			for (uint32 i = 1; i < numItemsRemaining; ++i)
			{
				SGfx_RenderQueueItem& nextItem = items[i];
				if (nextItem.mRasterizerState != rasterState ||
					nextItem.mBlendState != blendState ||
					nextItem.mVertexBuffer != vertexBuffer ||
					nextItem.mIndexBuffer != indexBuffer ||
					batchSize != SC_UINT16_MAX)
					break;

				// Add instance data
				++batchSize;
			}
		}

		firstItem.mBatchedSize = uint16(batchSize);
		items += batchSize;
		numItemsRemaining -= batchSize;
	}

	mIsPrepared = true;
}

void SGfx_RenderQueue::Render(SR_CommandList* aCmdList)
{
	uint32 numItemsRemaining = mItems.Count();
	if (!numItemsRemaining)
		return;

	assert(mIsPrepared);

	// Create temp instance buffer data

	const SGfx_RenderQueueItem* items = mItems.GetBuffer();
	uint32 instanceIndex = 0;
	while (numItemsRemaining)
	{
		const SGfx_RenderQueueItem& item = *items;
		uint32 batchSize = item.mBatchedSize;

		// Prefetch??

		SR_BufferResource* vertexBuffer = item.mVertexBuffer;
		SR_BufferResource* indexBuffer = item.mIndexBuffer;
		if (vertexBuffer && indexBuffer)
		{
			//aCmdList->SetRasterizerState(item.mRasterizerState);
			//aCmdList->SetBlendState(item.mBlendState);

			// Depth State?

			// Set InstanceData buffer

			aCmdList->SetVertexBuffer(vertexBuffer);
			aCmdList->SetIndexBuffer(indexBuffer);
			aCmdList->SetShaderState(item.mShader);
			aCmdList->SetPrimitiveTopology(SR_PrimitiveTopology::TriangleList);
			aCmdList->DrawIndexedInstanced(indexBuffer->GetProperties().mElementCount, batchSize);
			instanceIndex += batchSize;
		}

		items += batchSize;
		numItemsRemaining -= batchSize;
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
