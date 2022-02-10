#pragma once

/*
*
*	User can allocate a set of uints from this ring buffer to hold resource binding info (eg. descriptor indices)
* 
*/

class SR_InstanceBuffer
{
public:
	SR_InstanceBuffer();
	~SR_InstanceBuffer();

	bool Init(uint32 aInitialCount, const char* aDebugName = nullptr);

	void Begin();
	void* Alloc(uint32 aNumBytes, uint32& aOutOffset); // Returns begin gpu-address to newly allocated data
	void End();

private:
	void Grow(uint32 aNewSize);

	uint64 mCurrentAddress;
	uint64 mOffsetStride;

	void* mMappedDataPtr;

	SC_Ref<SR_BufferResource> mBufferResource;
	bool mIsActive : 1;
};

