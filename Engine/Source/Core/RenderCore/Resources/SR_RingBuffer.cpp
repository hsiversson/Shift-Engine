#include "SR_RingBuffer.h"
#include "RenderCore/Interface/SR_BufferResource.h"

SR_RingBuffer::SR_RingBuffer(const SC_Ref<SR_BufferResource>& aBuffer /*= nullptr*/, uint32 aAlignment /*= 1*/)
	: mBuffer(aBuffer)
	, mSize(0)
	, mAllocBlockMaxSize(0)
	, mAlignment(aAlignment)
	, mBeginOffset(0)
	, mEndOffset(0)
	, mOffsetBase(0)
	, mLatestFinishedFrame(SC_UINT64_MAX)
	, mLatestUpdatedFrame(SC_UINT64_MAX)
	, mLatestAllocFrame(SC_UINT64_MAX)
{
	if (mBuffer)
	{
		const SR_BufferResourceProperties& props = aBuffer->GetProperties();
		mSize = props.mElementCount * props.mElementSize;
		mAllocBlockMaxSize = mSize / 4;
	}
}

SR_RingBuffer::SR_RingBuffer(SR_RingBuffer&& /*aOther*/)
{

}

SR_RingBuffer::~SR_RingBuffer()
{

}

void SR_RingBuffer::operator=(SR_RingBuffer&& /*aOther*/)
{

}

bool SR_RingBuffer::GetOffset(uint32& aOutOffset, uint32 aSize, uint32 aAlignment /*= 0*/, const SR_Fence& aFence /*= SR_Fence()*/)
{
	assert((aSize & (mAlignment - 1)) == 0);

	if (aSize == 0)
	{
		aOutOffset = 0;
		return true;
	}

	uint32 alignedSize = aSize;
	uint32 maxMisalignment = 0;
	if (aAlignment > 1)
	{
		assert(SC_IsPow2(aAlignment));
		if (aAlignment > mAlignment)
		{
			maxMisalignment = aAlignment - mAlignment;
			alignedSize = aSize + maxMisalignment;
		}
	}

	if (alignedSize > mAllocBlockMaxSize)
	{
		assert(mAllocBlockMaxSize || !mSize);
		return false;
	}

	UpdateFrame(false, aFence);

	if (SR_RenderDevice::gInstance->mLatestFinishedFrame != mLatestFinishedFrame)
		Update();

	uint32 remainingBytes = mBeginOffset - mEndOffset;
	if (alignedSize > remainingBytes)
		return false; // full

	if (((mOffsetBase + mEndOffset + alignedSize) > mSize) && ((mOffsetBase + mEndOffset) <= mSize))
	{
		remainingBytes = mOffsetBase - mSize + mOffsetBase;
		if (alignedSize > remainingBytes)
			return false; // full

		mEndOffset += mSize - (mOffsetBase + mEndOffset);
	}

	UpdateFrame(true, aFence);

	aOutOffset = (mOffsetBase + mEndOffset) % mSize;
	if (maxMisalignment)
		aOutOffset = SC_Align(aOutOffset, aAlignment);

	mEndOffset += alignedSize;
	return true;
}

void SR_RingBuffer::Update()
{
	uint64 latestFinished = SR_RenderDevice::gInstance->mLatestFinishedFrame;

	while (mInFlightFrames.Count())
	{
		FrameData& frameInFlight = mInFlightFrames.Peek();
		if (frameInFlight.mFrame > latestFinished || SR_RenderDevice::gInstance->IsFencePending(frameInFlight.mFence))
			break;

		mBeginOffset += frameInFlight.mFirstOffset;
		mInFlightFrames.Remove();
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

		//RecenterBase
		mLatestUpdatedFrame = frameIndex;

		if (aAllocationFlag)
		{
			mLatestAllocFrame = frameIndex;
			mLatestAllocFence = aFence;
		}
	}
}
