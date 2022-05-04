#include "SR_TempResourceHeap.h"

SR_TempResourceHeap::SR_TempResourceHeap()
{

}

SR_TempResourceHeap::~SR_TempResourceHeap()
{

}

template<class T>
inline void EndFrameQueueAndList(SC_Array<T>& aList, SC_Queue<SC_Pair<SR_Fence, SC_Array<T>>>& aQueue, const SR_Fence& aFence)
{
	while (!aQueue.empty())
	{
		SC_Pair<SR_Fence, SC_Array<T>>& list = aQueue.front();
		if (list.mFirst.IsPending())
			break;

		aQueue.pop();
	}

	aQueue.push(SC_Pair(aFence, aList));
	aList.RemoveAll();
}

void SR_TempResourceHeap::EndFrame()
{
	SR_Fence fence = SR_RenderDevice::gInstance->GetCommandQueueManager()->InsertFence(SR_CommandListType::Graphics);

	EndFrameInternal();
	EndFrameQueueAndList(mTextureKeepAliveList, mTempTextureRemovalQueue, fence);
	EndFrameQueueAndList(mBufferKeepAliveList, mTempBufferRemovalQueue, fence);
}

SR_TempTexture SR_TempResourceHeap::GetTexture(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture, bool aIsRenderTarget, bool aIsWritable)
{
	return mTextureKeepAliveList.Add(GetTextureInternal(aTextureProperties, aIsTexture, aIsRenderTarget, aIsWritable));
}

SR_TempBuffer SR_TempResourceHeap::GetBuffer(const SR_BufferResourceProperties& aBufferProperties, bool aIsWritable)
{
	return mBufferKeepAliveList.Add(GetBufferInternal(aBufferProperties, aIsWritable));
}

void SR_TempResourceHeap::EndFrameInternal()
{
}
