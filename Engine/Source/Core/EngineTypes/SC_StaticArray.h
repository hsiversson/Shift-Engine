#pragma once

template<class T, SC_SizeT ArraySize>
class SC_StaticArray
{
public:
	SC_StaticArray() {}
	~SC_StaticArray() {}

	const SC_SizeT& Count()		{ return mCurrentItemCount; }
	const SC_SizeT& Capacity()	{ return ArraySize; }

	T& First()				{ return mInternalData[0]; }
	const T& First() const	{ return mInternalData[0]; }
	T& Last()				{ return mCurrentItemCount[mCurrentItemCount - 1]; }
	const T& Last() const	{ return mCurrentItemCount[mCurrentItemCount - 1]; }

	T* begin()				{ return &First(); }
	const T* begin() const	{ return &First(); }
	T* end()				{ return &Last(); }
	const T* end() const	{ return &Last(); }

	T& Add()
	{
		return mInternalData[0];
	}

	T& Add(const T& /*aItem*/)
	{
		return mInternalData[0];
	}

	T& Add(T&& /*aItem*/)
	{
		return mInternalData[0];
	}

	void RemoveAll()
	{

	}

	void RemoveLast()
	{

	}

	void RemoveCyclic(const T& /*aItem*/)
	{

	}

	void Remove(const T& /*aItem*/)
	{

	}

private:
	T mInternalData[ArraySize];
	SC_SizeT mCurrentItemCount;
};