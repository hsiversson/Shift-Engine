#pragma once
#include "SC_Message.h"
#include "Platform/Async/SC_Mutex.h"
#include <queue>

class SC_MessageListener;
class SC_MessageQueue
{
public:
	static SC_MessageQueue* Get();
	static void Destroy();
	static void QueueMessage(const SC_Message& aMessage);
	static void AddListener(SC_MessageListener* aListener);
	static void RemoveListener(SC_MessageListener* aListener);

	void Process();

private:
	SC_MessageQueue();
	~SC_MessageQueue();

	static SC_MessageQueue* gInstance;

	SC_Mutex mMutex;
	std::queue<SC_Message> mMessageQueue;
	std::queue<SC_Message> mMessageQueueTemp;
	SC_Array<SC_MessageListener*> mListeners;
};