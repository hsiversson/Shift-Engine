#include "SC_MessageQueue.h"
#include "SC_MessageListener.h"

SC_MessageQueue* SC_MessageQueue::gInstance = nullptr;

SC_MessageQueue* SC_MessageQueue::Get()
{
	if (gInstance == nullptr)
	{
		gInstance = new SC_MessageQueue;
	}
	return gInstance;
}

void SC_MessageQueue::Destroy()
{
	delete gInstance;
	gInstance = nullptr;
}

void SC_MessageQueue::QueueMessage(const SC_Message& aMessage)
{
	if (Get())
	{
		SC_MutexLock lock(gInstance->mMutex);
		gInstance->mMessageQueue.push(aMessage);
	}
}

void SC_MessageQueue::AddListener(SC_MessageListener* aListener)
{
	if (Get())
		gInstance->mListeners.AddUnique(aListener);
}

void SC_MessageQueue::RemoveListener(SC_MessageListener* aListener)
{
	if (Get())
		gInstance->mListeners.RemoveCyclic(aListener);
}

void SC_MessageQueue::Process()
{
	if (mMessageQueue.empty())
		return;

	{
		SC_MutexLock lock(mMutex);
		SC_Swap(mMessageQueueTemp, mMessageQueue);
	}

	SC_Message msg;
	while (!mMessageQueueTemp.empty())
	{
		msg = mMessageQueueTemp.front();
		mMessageQueueTemp.pop();

		for (SC_MessageListener* listener : mListeners)
		{
			listener->RecieveMessage(msg);
		}
	}
}

SC_MessageQueue::SC_MessageQueue()
{

}

SC_MessageQueue::~SC_MessageQueue()
{

}

