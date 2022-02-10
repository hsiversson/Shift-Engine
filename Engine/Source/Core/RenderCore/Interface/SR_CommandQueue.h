#pragma once
#include "SR_CommandList.h"
#include "SR_Fence.h"

class SR_CommandQueue
{
public:

	virtual SR_Fence SubmitCommandList(SR_CommandList* aCommandList, const char* aEventName = nullptr);
	virtual SR_Fence SubmitCommandLists(SR_CommandList** aCommandLists, uint32 aNumCommandLists, const char* aEventName = nullptr);
	virtual SR_Fence InsertFence();
	virtual SR_Fence InsertFence(const SC_Ref<SR_FenceResource>& aFence);
	virtual void InsertWait(const SR_Fence& aFence);

	virtual void BeginEvent(const char* aName);
	virtual void EndEvent();

	const SR_CommandListType& GetType() const;
protected:
	SR_CommandQueue();
	virtual ~SR_CommandQueue();

	SR_CommandListType mType;
};

