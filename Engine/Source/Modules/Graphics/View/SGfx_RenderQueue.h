#pragma once
#include "Graphics/View/SGfx_RenderQueueItem.h"

class SR_CommandList;
struct SGfx_ViewData;

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

	void Prepare(SGfx_ViewData& aPrepareData);
	void Render(SR_CommandList* aCmdList) const;
	void Clear();

	SGfx_RenderQueueItem& AddItem();
	SGfx_RenderQueueItem& AddItem(const SGfx_RenderQueueItem& aItem);

	uint32 NumItems() const;
	bool IsEmpty() const;

private:
	struct alignas(16) InstanceData
	{
		SC_Matrix mTransform;
		SC_Matrix mPrevTransform;
		uint32 mMaterialIndex;
		uint32 __pad[3];
	};

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