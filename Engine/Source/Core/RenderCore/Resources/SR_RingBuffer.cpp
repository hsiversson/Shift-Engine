#include "SR_RingBuffer.h"
#include "RenderCore/Interface/SR_BufferResource.h"

SR_RingBuffer::SR_RingBuffer(uint64 aSize, uint64 aAlignment /*= 1*/)
	: mSize(aSize)
	, mAllocBlockMaxSize(aSize/4)
	, mAlignment(aAlignment)
	, mBeginOffset(mSize)
	, mEndOffset(0)
	, mOffsetBase(0)
	, mLatestFinishedFrame(SC_UINT64_MAX)
	, mLatestUpdatedFrame(SC_UINT64_MAX)
	, mLatestAllocFrame(SC_UINT64_MAX)
{
}

SR_RingBuffer::SR_RingBuffer(SR_RingBuffer&& aOther)
	: mSize(aOther.mSize)
	, mAllocBlockMaxSize(aOther.mAllocBlockMaxSize)
	, mAlignment(aOther.mAlignment)
	, mBeginOffset(aOther.mBeginOffset)
	, mEndOffset(aOther.mEndOffset)
	, mOffsetBase(aOther.mOffsetBase)
	, mLatestFinishedFrame(aOther.mLatestFinishedFrame)
	, mLatestUpdatedFrame(aOther.mLatestUpdatedFrame)
	, mLatestAllocFrame(aOther.mLatestAllocFrame)
{
	aOther.mSize = 0;
	aOther.mAllocBlockMaxSize = 0;
	aOther.mAlignment = 0;
	aOther.mBeginOffset = 0;
	aOther.mEndOffset = 0;
	aOther.mOffsetBase = 0;
	aOther.mLatestFinishedFrame = SC_UINT64_MAX;
	aOther.mLatestUpdatedFrame = SC_UINT64_MAX;
	aOther.mLatestAllocFrame = SC_UINT64_MAX;
}

SR_RingBuffer::~SR_RingBuffer()
{

}

void SR_RingBuffer::operator=(SR_RingBuffer&& /*aOther*/)
{

}

bool SR_RingBuffer::GetOffset(uint64& aOutOffset, uint64 aSize, uint64 aAlignment /*= 0*/, const SR_Fence& aFence /*= SR_Fence()*/)
{
	SC_MutexLock lock(mMutex);
	SC_ASSERT((aSize & (mAlignment - 1)) == 0);

	if (aSize == 0)
	{
		aOutOffset = 0;
		return true;
	}

	uint64 alignedSize = aSize;
	uint64 maxMisalignment = 0;
	if (aAlignment > 1)
	{
		SC_ASSERT(SC_IsPow2(aAlignment));
		if (aAlignment > mAlignment)
		{
			maxMisalignment = aAlignment - mAlignment;
			alignedSize = aSize + maxMisalignment;
		}
	}

	if (alignedSize > mAllocBlockMaxSize)
	{
		SC_ASSERT(mAllocBlockMaxSize || !mSize);
		return false;
	}

	UpdateFrame(false, aFence);

	if (SR_RenderDevice::gInstance->mLatestFinishedFrame != mLatestFinishedFrame)
		Update();

	uint64 remainingBytes = mBeginOffset - mEndOffset;
	if (alignedSize > remainingBytes)
		return false; // full

	if (((mOffsetBase + mEndOffset + alignedSize) > mSize) && ((mOffsetBase + mEndOffset) <= mSize))
	{
		remainingBytes = mOffsetBase - mSize + mOffsetBase;
		if (alignedSize > remainingBytes)
			return false; // full

		SC_Atomic::Add(mEndOffset, mSize - (mOffsetBase + mEndOffset));
	}

	UpdateFrame(true, aFence);

	aOutOffset = (mOffsetBase + mEndOffset) % mSize;
	if (maxMisalignment)
		aOutOffset = SC_Align(aOutOffset, aAlignment);

	SC_Atomic::Add(mEndOffset, alignedSize);

	//SC_LOG("Allocated temp resource with size ({}) [Frame: {}].", aSize, SC_Time::gFrameCounter);
	return true;
}

void SR_RingBuffer::Update()
{
	uint64 latestFinished = SR_RenderDevice::gInstance->mLatestFinishedFrame;

	while (mInFlightFrames.Count())
	{
		FrameData& frameInFlight = mInFlightFrames.Peek();
		if (frameInFlight.mFrame > latestFinished || frameInFlight.mFence.IsPending())
			break;

		SC_Atomic::Add(mBeginOffset, frameInFlight.mFirstOffset);
		mInFlightFrames.Remove();

		//SC_LOG("Released temp resources with size [Frame: {}].", frameInFlight.mFrame);
	}

	mLatestFinishedFrame = latestFinished;
}

void SR_RingBuffer::UpdateFrame(bool aAllocationFlag, const SR_Fence& aFence)
{
	uint64 frameIndex = SC_Time::gFrameCounter;
	if (frameIndex != mLatestUpdatedFrame || (aAllocationFlag && (frameIndex != mLatestAllocFrame || aFence != mLatestAllocFence)))
	{
		FrameData frame;
		frame.mFence = mLatestAllocFence;
		frame.mFrame = mLatestUpdatedFrame != SC_UINT64_MAX ? mLatestUpdatedFrame : 0;
		frame.mFirstOffset = mEndOffset;
		mInFlightFrames.Add(frame);

		RecenterBase();
		mLatestUpdatedFrame = frameIndex;

		if (aAllocationFlag)
		{
			mLatestAllocFrame = frameIndex;
			mLatestAllocFence = aFence;
		}
	}
}

void SR_RingBuffer::RecenterBase()
{
	mOffsetBase += mEndOffset;
	mBeginOffset -= mEndOffset;
	mEndOffset = 0;
	mOffsetBase = mOffsetBase % mSize;
}
