#pragma once
#include "RenderCore/Defines/SR_RenderDefines.h"

struct ID3D12Resource;
class SR_ResourceDelayDestructor
{
public:
	SR_ResourceDelayDestructor();
	~SR_ResourceDelayDestructor();

	bool AddToQueue(SR_Resource* aResource);
	void Run();

	static const bool gEnabled = true;
private:
	struct PendingDestruct
	{
		SR_Resource* mResource;
		uint32 mDestructionFrame;
	};
	SC_RingArray<PendingDestruct> mQueue;
	SC_Mutex mMutex;
};

