#pragma once
#include "RenderCore/Defines/SR_RenderEnums.h"

struct SR_Rect
{
	SR_Rect() : mLeft(0), mTop(0), mRight(0), mBottom(0) {}
	SR_Rect(const std::initializer_list<uint32>& aList) : mLeft(0), mTop(0), mRight(0), mBottom(0) { SC_Memcpy(&mLeft, aList.begin(), sizeof(uint32) * aList.size()); }

	SC_IntVector2 TopLeft() const { return SC_IntVector2((int32)mLeft, (int32)mTop); }
	SC_IntVector2 BottomRight() const { return SC_IntVector2((int32)mRight, (int32)mBottom); }
	SC_IntVector2 Size() const { return SC_IntVector2((int32)mRight - (int32)mLeft, (int32)mBottom - (int32)mTop); }

	uint32 mLeft;
	uint32 mTop;
	uint32 mRight;
	uint32 mBottom;
};

struct SR_SamplerProperties
{
	SR_SamplerProperties(SR_FilterMode aFilter = SR_FilterMode::Linear, SR_WrapMode aWrapMode = SR_WrapMode::Wrap)
		: mBorderColor(0), mLODBias(0)
		, mMinFilter(aFilter), mMagFilter(aFilter), mMipFilter(aFilter)
		, mWrapX(aWrapMode), mWrapY(aWrapMode), mWrapZ(aWrapMode)
		, mMaxAnisotropy(gDefaultAnisotropy), mComparison(SR_ComparisonFunc::Never)
	{}

	bool operator==(const SR_SamplerProperties& aOther) const
	{
		return mBorderColor == aOther.mBorderColor && mLODBias == aOther.mLODBias &&
			mMinFilter == aOther.mMinFilter && mMagFilter == aOther.mMagFilter && mMipFilter == aOther.mMipFilter &&
			mWrapX == aOther.mWrapX && mWrapY == aOther.mWrapY && mWrapZ == aOther.mWrapZ &&
			mMaxAnisotropy == aOther.mMaxAnisotropy && mComparison == aOther.mComparison;
	}

	bool operator!=(const SR_SamplerProperties& aOther) const 
	{ 
		return !(*this == aOther); 
	}

	SC_Vector4 mBorderColor;
	float mLODBias;

	SR_FilterMode mMinFilter;
	SR_FilterMode mMagFilter;
	SR_FilterMode mMipFilter;

	SR_WrapMode mWrapX;
	SR_WrapMode mWrapY;
	SR_WrapMode mWrapZ;

	uint8 mMaxAnisotropy;

	SR_ComparisonFunc mComparison;

	static constexpr uint8 gDefaultAnisotropy = 255;
};