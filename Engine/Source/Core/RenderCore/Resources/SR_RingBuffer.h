#pragma once
#include "RenderCore/Interface/SR_Fence.h"

class SR_BufferResource;

class SR_RingBuffer
{
public:
	struct FrameData
	{
		SR_Fence mFence;
		uint64 mFrame;
		uint64 mFirstOffset;
	};

public:
	SR_RingBuffer(const SC_Ref<SR_BufferResource>& aBufferResource = nullptr, uint64 aAlignment = 1);
	SR_RingBuffer(uint64 aSize, uint64 aAlignment = 1);
	SR_RingBuffer(const SR_RingBuffer&) = delete;
	SR_RingBuffer(SR_RingBuffer&& aOther);
	~SR_RingBuffer();

	void operator=(const SR_RingBuffer&) = delete;
	void operator=(SR_RingBuffer&& aOther);

	bool GetOffset(uint64& aOffsetOut, uint64 aSize, uint64 aAlignment = 0, const SR_Fence& aFence = SR_Fence());
	void Update();
	void UpdateFrame(bool aAllocationFlag, const SR_Fence& aFence);

	SR_BufferResource* GetBufferResource() const;

	void RecenterBase();

	SC_Mutex mMutex;
	SC_RingArray<FrameData> mInFlightFrames;
	SC_Ref<SR_BufferResource> mBuffer;
	uint64 mSize;
	uint64 mAllocBlockMaxSize;
	uint64 mAlignment;

	volatile uint64 mBeginOffset;
	volatile uint64 mEndOffset;
	volatile uint64 mOffsetBase;

	volatile uint64 mLatestFinishedFrame;
	volatile uint64 mLatestUpdatedFrame;
	volatile uint64 mLatestAllocFrame;
	SR_Fence mLatestAllocFence;
};