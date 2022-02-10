#pragma once

/* 
	Used to create temporary resources on the GPU upload heap which are useful for CPU to GPU data transfer.
*/

class SR_Heap;
class SR_UploadHeap
{
public:
	SR_UploadHeap();
	~SR_UploadHeap();

	bool Init();

	void CopyTexture(SR_TextureResource* aTargetResource, const void* aData, uint64 aSize, uint64 aOffset = 0);
	void CopyBuffer(SR_BufferResource* aTargetResource, const void* aData, uint64 aSize, uint64 aOffset = 0);

private:
	SC_Ref<SR_Heap> mHeap;
};

