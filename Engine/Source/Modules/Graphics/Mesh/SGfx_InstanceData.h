#pragma once

struct SR_TempBuffer;

class SGfx_InstanceData
{
public:
	SGfx_InstanceData();
	~SGfx_InstanceData();

	void Add(uint32& aOffsetOut, uint32 aCount, const SC_Vector4* aData);
	void Clear();

	SR_TempBuffer GetBuffer() const;

private:
	SC_Array<SC_Vector4> mData;
	SC_Mutex mMutex;
	uint32 mCurrentOffset;
};

