#pragma once

enum class SR_HeapResourceType
{
	Buffer,
	Texture,
	RenderTarget,
	//_Debug,
	COUNT
};

enum class SR_HeapType
{
	Default,
	Upload,
	Readback,
	COUNT
};

struct SR_HeapProperties
{
	uint64 mByteSize;
	SR_HeapResourceType mResourceType;
	SR_HeapType mType;
	bool mCreateResourcesUninitialized;
};

class SR_Heap
{
public:
	SR_Heap(const SR_HeapProperties& aProperties);
	virtual ~SR_Heap();

	virtual const uint64 GetOffset(uint64 aSize, uint64 aAlignment) = 0;

	const SR_HeapProperties& GetProperties() const;

protected:
	SR_HeapProperties mProperties;
};

