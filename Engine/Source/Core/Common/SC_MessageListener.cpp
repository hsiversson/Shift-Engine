#include "SC_MessageListener.h"
#include "SC_MessageQueue.h"

SC_MessageListener::SC_MessageListener()
{
	SC_MessageQueue::AddListener(this);
}

SC_MessageListener::~SC_MessageListener()
{
	SC_MessageQueue::RemoveListener(this);
}

