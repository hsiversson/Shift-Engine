#include "SR_TempResourceHeap.h"

SR_TempResourceHeap::SR_TempResourceHeap()
{

}

SR_TempResourceHeap::~SR_TempResourceHeap()
{

}

void SR_TempResourceHeap::EndFrame()
{
	while (!mRemovalQueue.empty())
	{
		SC_Pair<SR_Fence, SC_Array<SR_TempTexture>>& list = mRemovalQueue.front();
		if (list.mFirst.IsPending())
			break;

		mRemovalQueue.pop();
	}

	EndFrameInternal();
	
	SR_Fence fence = SR_RenderDevice::gInstance->GetGraphicsCommandQueue()->InsertFence();
	mRemovalQueue.push(SC_Pair(fence, mKeepAliveList));
	mKeepAliveList.RemoveAll();
}

SR_TempTexture SR_TempResourceHeap::GetTexture(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture, bool aIsRenderTarget, bool aIsWritable)
{
	return mKeepAliveList.Add(GetTextureInternal(aTextureProperties, aIsTexture, aIsRenderTarget, aIsWritable));
}

void SR_TempResourceHeap::EndFrameInternal()
{
}
