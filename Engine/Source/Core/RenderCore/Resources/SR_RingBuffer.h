#pragma once
#include "Interface/SR_Fence.h"

class SR_BufferResource;

class SR_RingBuffer
{
public:
	struct FrameData
	{
		SR_Fence mFence;
		uint64 mFrame;
		uint32 mFirstOffset;
	};

public:
	SR_RingBuffer(const SC_Ref<SR_BufferResource>& aBuffer = nullptr, uint32 aAlignment = 1);
	SR_RingBuffer(const SR_RingBuffer&) = delete;
	SR_RingBuffer(SR_RingBuffer&& aOther);
	~SR_RingBuffer();

	void operator=(const SR_RingBuffer&) = delete;
	void operator=(SR_RingBuffer&& aOther);


	bool GetOffset(uint32& aOffsetOut, uint32 aSize, uint32 aAlignment = 0, const SR_Fence& aFence = SR_Fence());
	void Update();
	void UpdateFrame(bool aAllocationFlag, const SR_Fence& aFence);

private:
	SC_RingArray<FrameData> mInFlightFrames;
	SC_Ref<SR_BufferResource> mBuffer;
	uint32 mSize;
	uint32 mAllocBlockMaxSize;
	uint32 mAlignment;

	uint32 mBeginOffset;
	uint32 mEndOffset;
	uint32 mOffsetBase;

	uint64 mLatestFinishedFrame;
	uint64 mLatestUpdatedFrame;
	uint64 mLatestAllocFrame;
	SR_Fence mLatestAllocFence;
};