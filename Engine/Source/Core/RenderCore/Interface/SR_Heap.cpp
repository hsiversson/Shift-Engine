#include "SR_Heap.h"

SR_Heap::SR_Heap(const SR_HeapProperties& aProperties)
	: mProperties(aProperties)
{

}

SR_Heap::~SR_Heap()
{

}

const SR_HeapProperties& SR_Heap::GetProperties() const
{
	return mProperties;
}
