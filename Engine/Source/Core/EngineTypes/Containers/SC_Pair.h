#pragma once

template<class T1, class T2>
class SC_Pair
{
public:
	SC_Pair()
	{
	}

	SC_Pair(const T1& aFirst, const T2& aSecond)
		: mFirst(aFirst), mSecond(aSecond)
	{
	}

	SC_Pair(T1&& aFirst, T2&& aSecond)
		: mFirst(SC_Move(aFirst)), mSecond(SC_Move(aSecond))
	{
	}

	SC_Pair(const T1& aFirst, T2&& aSecond)
		: mFirst(aFirst), mSecond(SC_Move(aSecond))
	{
	}

	SC_Pair(T1&& aFirst, const T2& aSecond)
		: mFirst(SC_Move(aFirst)), mSecond(aSecond)
	{
	}

	template<typename OtherType1, typename OtherType2>
	SC_Pair(const SC_Pair<OtherType1, OtherType2>& aPair)
		: mFirst(static_cast<T1>(aPair.mFirst)), mSecond(static_cast<T2>(aPair.mSecond))
	{
	}

	SC_Pair(const SC_Pair<T1, T2>& aPair)
		: mFirst(aPair.mFirst), mSecond(aPair.mSecond)
	{
	}

	SC_Pair<T1, T2>& operator=(const SC_Pair<T1, T2>& aPair)
	{
		mFirst = aPair.mFirst;
		mSecond = aPair.mSecond;
		return *this;
	}

	SC_Pair(SC_Pair<T1, T2>&& aPair) noexcept
		: mFirst(SC_Move(aPair.mFirst)), mSecond(SC_Move(aPair.mSecond))
	{
	}

	SC_Pair<T1, T2>& operator=(SC_Pair<T1, T2>&& aPair) noexcept
	{
		mFirst = SC_Move(aPair.mFirst);
		mSecond = SC_Move(aPair.mSecond);
		return *this;
	}

	bool operator==(const SC_Pair& aPair) const
	{
		return mFirst == aPair.mFirst && mSecond == aPair.mSecond;
	}

	bool operator!=(const SC_Pair& aPair) const
	{
		return !(*this == aPair);
	}

	bool operator<(const SC_Pair& aPair) const
	{
		return mFirst != aPair.mFirst ? mFirst < aPair.mFirst : mSecond < aPair.mSecond;
	}

	bool operator>(const SC_Pair& aPair) const
	{
		return mFirst != aPair.mFirst ? mFirst > aPair.mFirst : mSecond > aPair.mSecond;
	}

	T1 mFirst;
	T2 mSecond;
};