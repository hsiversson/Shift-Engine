
#include "SR_CommandQueue.h"

SR_Fence SR_CommandQueue::SubmitCommandList(SR_CommandList* aCommandList, const char* aEventName)
{
	return SubmitCommandLists(&aCommandList, 1, aEventName);
}

SR_Fence SR_CommandQueue::SubmitCommandLists(SR_CommandList** /*aCommandLists*/, uint32 /*aNumCommandLists*/, const char* /*aEventName*/)
{
	return SR_Fence();
}

SR_Fence SR_CommandQueue::InsertFence()
{
	return SR_Fence();
}

SR_Fence SR_CommandQueue::InsertFence(const SC_Ref<SR_FenceResource>& /*aFence*/)
{
	return SR_Fence();
}

void SR_CommandQueue::InsertWait(const SR_Fence& /*aFence*/)
{

}

void SR_CommandQueue::BeginEvent(const char* /*aName*/)
{
}

void SR_CommandQueue::EndEvent()
{
}

const SR_CommandListType& SR_CommandQueue::GetType() const
{
	return mType;
}

SR_CommandQueue::SR_CommandQueue()
	: mType(SR_CommandListType::Unknown)
{

}

SR_CommandQueue::~SR_CommandQueue()
{

}
