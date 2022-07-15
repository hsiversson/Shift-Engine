#pragma once

template<class T, SC_SizeT Size>
class SC_StaticArray
{
public:
	SC_StaticArray() {}
	~SC_StaticArray() {}

	SC_SizeT Count() const { return Size; }
	SC_SizeT Capacity() const { return Size; }
	SC_SizeT GetByteSize() const { return Size * sizeof(T); }

	T& First() { return mInternalData[0]; }
	T& Last() { return mInternalData[Size - 1]; }
	const T& First() const { return mInternalData[0]; }
	const T& Last() const { return mInternalData[Size - 1]; }

	T* begin() { return &mInternalData[0]; }
	T* end() { return &mInternalData[Size - 1]; }
	const T* begin() const { return &mInternalData[0]; }
	const T* end() const { return &mInternalData[Size - 1]; }

	const T& operator[] (uint32 aIndex) const
	{
		SC_ASSERT(aIndex < Size, "Out of bounds! (anIndex: {0} Size: {1})", aIndex, Size);
		return mInternalData[aIndex];
	}

	T& operator[] (uint32 aIndex)
	{
		SC_ASSERT(aIndex < Size, "Out of bounds! (anIndex: {0} Size: {1})", aIndex, Size);
		return mInternalData[aIndex];
	}

private:
	T mInternalData[Size];
};