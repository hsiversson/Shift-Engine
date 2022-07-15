#include "SR_TempResourceHeap.h"

SR_TempResourceHeap::SR_TempResourceHeap()
{

}

SR_TempResourceHeap::~SR_TempResourceHeap()
{

}

template<class T>
inline void EndFrameQueueAndList(SC_Array<T>& aList, SC_Queue<SC_Pair<uint32, SC_Array<T>>>& aQueue, uint32 aFrame)
{
	while (!aQueue.empty())
	{
		const SC_Pair<uint32, SC_Array<T>>& list = aQueue.front();
		const uint32 frameIndexToDelete = list.mFirst;
		if (frameIndexToDelete > SC_Time::gFrameCounter)
			break;

		aQueue.pop();
	}

	aQueue.push(SC_Pair(aFrame, aList));
	aList.RemoveAll();
}

void SR_TempResourceHeap::EndFrame()
{
	uint32 deletionFrame = SC_Time::gFrameCounter + 1;
	EndFrameQueueAndList(mTextureKeepAliveList, mTempTextureRemovalQueue, deletionFrame);
	EndFrameQueueAndList(mBufferKeepAliveList, mTempBufferRemovalQueue, deletionFrame);
	EndFrameInternal();
}

SR_TempTexture SR_TempResourceHeap::GetTexture(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture, bool aIsRenderTarget, bool aIsWritable)
{
	SR_TempTexture tempTex = GetTextureInternal(aTextureProperties, aIsTexture, aIsRenderTarget, aIsWritable); 
	mTextureKeepAliveList.Add(tempTex);
	return tempTex;
}

SR_TempBuffer SR_TempResourceHeap::GetBuffer(const SR_BufferResourceProperties& aBufferProperties, bool aIsWritable)
{
	SR_TempBuffer tempBuf = GetBufferInternal(aBufferProperties, aIsWritable);
	mBufferKeepAliveList.Add(tempBuf);
	return tempBuf;
}

void SR_TempResourceHeap::EndFrameInternal()
{
}
