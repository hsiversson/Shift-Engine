#pragma once


class SGfx_InstanceData
{
public:
	SGfx_InstanceData();
	~SGfx_InstanceData();

	void Add(uint32 aCount, SC_Vector4* aData);

private:
	SC_Array<SC_Vector4> mData;

};

