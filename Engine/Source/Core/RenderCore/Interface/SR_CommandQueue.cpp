#include "SR_CommandQueue.h"

void SR_CommandQueue::SubmitCommandList(SR_CommandList* aCommandList, const char* aEventName)
{
	SubmitCommandLists(&aCommandList, 1, aEventName);
}

void SR_CommandQueue::SubmitCommandLists(SR_CommandList** /*aCommandLists*/, uint32 /*aNumCommandLists*/, const char* /*aEventName*/)
{
}

SR_Fence SR_CommandQueue::GetNextFence()
{
	return SR_Fence();
}

void SR_CommandQueue::SignalFence(const SR_Fence& /*aFence*/)
{
}

void SR_CommandQueue::InsertWait(const SR_Fence& /*aFence*/)
{
}

bool SR_CommandQueue::IsFencePending(const SR_Fence& /*aFence*/)
{
	return false;
}

bool SR_CommandQueue::WaitForFence(const SR_Fence& /*aFence*/, bool /*aBlock*/)
{
	return false;
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

const char* SR_CommandQueue::GetTypeName(const SR_CommandListType& aType)
{
	switch (aType)
	{
	case SR_CommandListType::Graphics:
		return "Graphics";
	case SR_CommandListType::Compute:
		return "Compute";
	case SR_CommandListType::Copy:
		return "Copy";
	}
	return "Unknown";
}

SR_CommandQueue::SR_CommandQueue()
	: mType(SR_CommandListType::Unknown)
{

}

SR_CommandQueue::~SR_CommandQueue()
{

}
