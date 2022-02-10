#pragma once

class SC_BlockAllocator
{
public:
	SC_BlockAllocator(uint64 aBlockByteSize, uint32 aNumBlocks);
	~SC_BlockAllocator();

	uint64 GetMemoryUsage() const;

private:
	const uint64 mBlockSize;
	const uint32 mBlockCount;

	uint32 mNumBlocksUsed;
};

