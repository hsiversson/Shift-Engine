#pragma once
#include "SR_CommandList.h"
#include "SR_Fence.h"

class SR_CommandQueue
{
public:
	virtual void SubmitCommandList(SR_CommandList* aCommandList, const char* aEventName = nullptr);
	virtual void SubmitCommandLists(SR_CommandList** aCommandLists, uint32 aNumCommandLists, const char* aEventName = nullptr);

	virtual SR_Fence GetNextFence();

	virtual void SignalFence(const SR_Fence& aFence);
	virtual void InsertWait(const SR_Fence& aFence);

	virtual bool IsFencePending(const SR_Fence& aFence);
	virtual bool WaitForFence(const SR_Fence& aFence, bool aBlock = true);

	virtual void BeginEvent(const char* aName);
	virtual void EndEvent();

	const SR_CommandListType& GetType() const;

	static const char* GetTypeName(const SR_CommandListType& aType);
protected:
	SR_CommandQueue();
	virtual ~SR_CommandQueue();

	SR_CommandListType mType;
};

