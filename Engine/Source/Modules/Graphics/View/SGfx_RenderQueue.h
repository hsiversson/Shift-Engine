#pragma once
#include "Graphics/View/SGfx_RenderQueueItem.h"

class SR_CommandList;

class SGfx_RenderQueue
{
public:
	enum class SortType
	{
		Default,
		ByState = Default,
		FarFirst,
		NearFirst
	};

	SGfx_RenderQueue(const SortType& aSortType);
	virtual ~SGfx_RenderQueue();

	void Prepare();
	void Render(SR_CommandList* aCmdList);
	void Clear();

	SGfx_RenderQueueItem& AddItem();
	SGfx_RenderQueueItem& AddItem(const SGfx_RenderQueueItem& aItem);

	uint32 NumItems() const;
	bool IsEmpty() const;

private:
	void Sort();
	void Sort_ByState();
	void Sort_FarFirst();
	void Sort_NearFirst();

private:
	SC_Array<SGfx_RenderQueueItem> mItems;
	const SortType mSortType;
	bool mIsPrepared;
};

class SGfx_RenderQueue_ByState : public SGfx_RenderQueue
{
public:
	SGfx_RenderQueue_ByState() : SGfx_RenderQueue(SGfx_RenderQueue::SortType::ByState) {}
};

class SGfx_RenderQueue_FarFirst : public SGfx_RenderQueue
{
public:
	SGfx_RenderQueue_FarFirst() : SGfx_RenderQueue(SGfx_RenderQueue::SortType::FarFirst) {}
};

class SGfx_RenderQueue_NearFirst : public SGfx_RenderQueue
{
public:
	SGfx_RenderQueue_NearFirst() : SGfx_RenderQueue(SGfx_RenderQueue::SortType::NearFirst) {}
};