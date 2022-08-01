#include "SR_TempResourceHeap.h"

SR_TempResourceHeap::SR_TempResourceHeap()
{

}

SR_TempResourceHeap::~SR_TempResourceHeap()
{

}

template<class T>
inline void EndFrameQueueAndList(SC_Array<T>& aList, SC_Queue<SC_Pair<SR_Fence, SC_Array<T>>>& aQueue, SR_Fence aFence)
{
	while (!aQueue.empty())
	{
		const SC_Pair<SR_Fence, SC_Array<T>>& list = aQueue.front();
		const SR_Fence fence = list.mFirst;
		if (fence.IsPending())
			break;

		aQueue.pop();
	}

	aQueue.push(SC_Pair(aFence, aList));
	aList.RemoveAll();
}

void SR_TempResourceHeap::EndFrame()
{
	SR_Fence deletionFence = SR_RenderDevice::gInstance->GetQueueManager()->InsertFence(SR_CommandListType::Graphics);
	EndFrameQueueAndList(mTextureKeepAliveList, mTempTextureRemovalQueue, deletionFence);
	EndFrameInternal();
}

SR_TempTexture SR_TempResourceHeap::GetTexture(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture, bool aIsRenderTarget, bool aIsWritable)
{
	SR_TempTexture tempTex = GetTextureInternal(aTextureProperties, aIsTexture, aIsRenderTarget, aIsWritable); 
	mTextureKeepAliveList.Add(tempTex);
	return tempTex;
}

SR_TempBuffer SR_TempResourceHeap::GetBuffer(const SR_BufferResourceProperties& /*aBufferProperties*/, bool /*aIsWritable*/)
{
	return SR_TempBuffer();
}

void SR_TempResourceHeap::EndFrameInternal()
{
}
