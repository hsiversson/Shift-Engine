#pragma once
#include "SGfx_Material.h"

class SGfx_Material;

struct SGfx_MaterialGPUData
{
	SGfx_MaterialGPUData()
		: multiplier(1.0f)
	{
		SC_Fill(mTextureIndices, SGfx_MaxMaterialTextureCount, SC_UINT32_MAX);
	}

	uint32 mTextureIndices[SGfx_MaxMaterialTextureCount];
	float multiplier;
};

class SGfx_MaterialCache
{
public:
	static SGfx_MaterialCache& Get();
	
	void Add(SC_Ref<SGfx_Material> aMaterial);
	bool TryAdd(SC_Ref<SGfx_Material> aMaterial);

private:

	SC_Array<SC_Ref<SGfx_Material>> mCachedMaterials;
};

class SGfx_MaterialGPUDataBuffer
{
public:
	static SGfx_MaterialGPUDataBuffer& Get();

	uint32 Add(const SGfx_MaterialGPUData& aMaterialData);
	void UpdateData(const SGfx_MaterialGPUData& aMaterialData, uint32 aMaterialIndex);
	void Remove(uint32 aIndex);

	void UpdateBuffer();
	uint32 GetBufferDescriptorIndex() const;

private:
	SGfx_MaterialGPUDataBuffer();
	~SGfx_MaterialGPUDataBuffer();

	uint32 GetIndex();
	void ReturnIndex(uint32 aIndex);

	static constexpr uint32 gInitialSize = 4096;
	SC_Array<SGfx_MaterialGPUData> mMaterialGPUDatas;
	SC_Array<uint32> mFreeIndices;
	SC_Mutex mMutex;

	SC_Ref<SR_BufferResource> mBufferResource;
	SC_Ref<SR_Buffer> mBuffer;

	bool mIsDirty : 1;
};