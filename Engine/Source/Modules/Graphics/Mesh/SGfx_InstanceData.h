#pragma once

struct SR_TempBuffer;

class SGfx_InstanceData
{
public:
	SGfx_InstanceData(const char* aDebugName = nullptr);
	~SGfx_InstanceData();

	void Add(uint32& aOffsetOut, uint32 aCount, const SC_Vector4* aData);
	void Clear();

	void Prepare();

	SR_Buffer* GetBuffer(); // Should only be called from a RenderTask really.

private:
	SC_Array<SC_Vector4> mData;
	SC_Ref<SR_BufferResource> mBufferResource;
	SC_Ref<SR_Buffer> mBuffer;
	SC_Mutex mMutex;
	uint32 mCurrentOffset;
	const char* mDebugName;
};

