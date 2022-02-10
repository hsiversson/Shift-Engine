#pragma once
#include "SC_Message.h"

class SC_MessageListener
{
	friend class SC_MessageQueue;
public:
	SC_MessageListener();
	virtual ~SC_MessageListener();

protected:
	virtual void RecieveMessage(const SC_Message& aMsg) = 0;
};