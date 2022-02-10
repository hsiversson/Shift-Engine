#include "SGfx_MaterialCache.h"

SGfx_MaterialCache& SGfx_MaterialCache::Get()
{
	static SGfx_MaterialCache gInstance;
	return gInstance;
}

void SGfx_MaterialCache::Add(SC_Ref<SGfx_Material> /*aMaterial*/)
{

}

bool SGfx_MaterialCache::TryAdd(SC_Ref<SGfx_Material> /*aMaterial*/)
{
	return false;
}

SGfx_MaterialGPUDataBuffer& SGfx_MaterialGPUDataBuffer::Get()
{
	static SGfx_MaterialGPUDataBuffer gInstance;
	return gInstance;
}

uint32 SGfx_MaterialGPUDataBuffer::Add(const SGfx_MaterialGPUData& aMaterialData)
{
	uint32 index = GetIndex();
	mMaterialGPUDatas[index] = aMaterialData;

	mIsDirty = true;
	return index;
}

void SGfx_MaterialGPUDataBuffer::UpdateData(const SGfx_MaterialGPUData& aMaterialData, uint32 aMaterialIndex)
{
	assert(aMaterialIndex < mMaterialGPUDatas.Count());
	mMaterialGPUDatas[aMaterialIndex] = aMaterialData;
	mIsDirty = true;
}

void SGfx_MaterialGPUDataBuffer::Remove(uint32 aIndex)
{
	ReturnIndex(aIndex);
}

uint32 SGfx_MaterialGPUDataBuffer::GetBufferDescriptorIndex() const
{
	return mBuffer->GetDescriptorHeapIndex();
}

SGfx_MaterialGPUDataBuffer::SGfx_MaterialGPUDataBuffer()
	: mIsDirty(true)
{
	mFreeIndices.Reserve(gInitialSize);
	for (int i = gInitialSize - 1; i >= 0; --i)
	{
		mFreeIndices.Add(i);
	}

	mMaterialGPUDatas.Respace(gInitialSize);

	SR_BufferResourceProperties bufferProps;
	bufferProps.mBindFlags = SR_BufferBindFlag_Buffer;
	bufferProps.mDebugName = "MaterialGPUDataBuffer";
	bufferProps.mElementSize = sizeof(SGfx_MaterialGPUData);
	bufferProps.mElementCount = gInitialSize;
	mBufferResource = SR_RenderDevice::gInstance->CreateBufferResource(bufferProps);
}

SGfx_MaterialGPUDataBuffer::~SGfx_MaterialGPUDataBuffer()
{
}

uint32 SGfx_MaterialGPUDataBuffer::GetIndex()
{
	uint32 index = 0;
	{
		SC_MutexLock lock(mMutex);

		assert((mFreeIndices.Count() > 0) && "Material info buffer is full.");
		index = mFreeIndices.Last();
		mFreeIndices.RemoveLast();
	}
	return index;
}

void SGfx_MaterialGPUDataBuffer::ReturnIndex(uint32 aIndex)
{
	SC_MutexLock lock(mMutex);
	mFreeIndices.Add(uint32(aIndex));
}

void SGfx_MaterialGPUDataBuffer::UpdateBuffer()
{
	if (!mIsDirty)
		return;

	mBufferResource->UpdateData(0, mMaterialGPUDatas.GetBuffer(), mMaterialGPUDatas.GetByteSize());

	SR_BufferProperties bufferProperties;
	bufferProperties.mElementCount = mMaterialGPUDatas.Count();
	bufferProperties.mFirstElement = 0;
	bufferProperties.mType = SR_BufferType::Structured;
	mBuffer = SR_RenderDevice::gInstance->CreateBuffer(bufferProperties, mBufferResource);

	mIsDirty = false;
}
