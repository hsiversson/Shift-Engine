#include "SC_BlockAllocator.h"
#include <memory>

SC_BlockAllocator::SC_BlockAllocator(uint64 aBlockByteSize, uint32 aNumBlocks)
	: mBlockSize(aBlockByteSize)
	, mBlockCount(aNumBlocks)
	, mNumBlocksUsed(0)
{
}

SC_BlockAllocator::~SC_BlockAllocator()
{
}

uint64 SC_BlockAllocator::GetMemoryUsage() const
{
	return mBlockSize * uint64(mNumBlocksUsed);
}
