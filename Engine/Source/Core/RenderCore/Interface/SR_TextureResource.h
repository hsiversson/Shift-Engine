#pragma once
#include "RenderCore/Defines/SR_Format.h"
#include "RenderCore/Resources/SR_Resource.h"

class SR_Heap;

static constexpr uint32 SR_MaxMipCount = 13;

enum SR_CubeMapFace : uint8
{
	SR_CUBE_FACE_POSITIVE_X,
	SR_CUBE_FACE_NEGATIVE_X,
	SR_CUBE_FACE_POSITIVE_Y,
	SR_CUBE_FACE_NEGATIVE_Y,
	SR_CUBE_FACE_POSITIVE_Z,
	SR_CUBE_FACE_NEGATIVE_Z,
};

struct SR_TextureLevel
{
	SR_TextureLevel() : mMipLevel(0), mArrayIndex(0), mFace(SR_CUBE_FACE_POSITIVE_X) {}
	bool operator==(const SR_TextureLevel& aOther) const { return mMipLevel == aOther.mMipLevel && mFace == aOther.mFace && mArrayIndex == aOther.mArrayIndex; }
	bool operator!=(const SR_TextureLevel& aOther) const { return !(*this == aOther); }

	uint32 mMipLevel;
	uint32 mArrayIndex;
	SR_CubeMapFace mFace;
};

struct SR_TextureRange
{
	SR_TextureRange() : mMostDetailedMip(0), mMipLevels(0), mFirstArrayIndex(0), mArraySize(0) {}

	bool operator==(const SR_TextureRange& aOther) const { return mMostDetailedMip == aOther.mMostDetailedMip && mMipLevels == aOther.mMipLevels && mFirstArrayIndex == aOther.mFirstArrayIndex && mArraySize == aOther.mArraySize; }
	bool operator!=(const SR_TextureRange& aOther) const { return !(*this == aOther); }

	uint8 mMostDetailedMip;
	uint8 mMipLevels;
	uint16 mFirstArrayIndex;
	uint16 mArraySize;
};

struct SR_TexturePixelRange
{
	SR_TextureLevel mLevel;
	uint32 mArraySize;
	uint32 mDepthSize;
	uint32 mDepthIndex;

	explicit SR_TexturePixelRange(uint32 aMipLevel = 0) : mArraySize(1), mDepthSize(1), mDepthIndex(0)
	{
		mLevel.mMipLevel = aMipLevel;
	}
};

struct SR_TextureResourceProperties
{
	SR_TextureResourceProperties()
		: mSize(0)
		, mFormat(SR_Format::UNKNOWN)
		, mType(SR_ResourceType::Unknown)
		, mNumMips(1)
		, mArraySize(1)
		, mHeap(nullptr)
		, mAllowRenderTarget(false)
		, mAllowDepthStencil(false)
		, mAllowUnorderedAccess(false)
		, mIsCubeMap(false)
		, mSourceFile("Unknown")
	{}

	SR_TextureResourceProperties(const SR_TextureResourceProperties& aProperties, uint32 aMipOffset);
	SR_TextureResourceProperties(const SR_TextureResourceProperties& aProperties, const SR_TexturePixelRange& aRange);


	SC_IntVector mSize;
	SR_Format mFormat;
	SR_ResourceType mType;
	uint16 mNumMips;
	uint16 mArraySize;

	SR_Heap* mHeap;

	bool mAllowRenderTarget;
	bool mAllowDepthStencil;
	bool mAllowUnorderedAccess;

	bool mIsCubeMap;

	SC_FilePath mSourceFile;
	std::string mDebugName;
};

struct SR_PixelData
{
	SR_PixelData() : mData(nullptr), mBytesPerLine(0), mBytesPerSlice(0), mSize(0) {}

	SR_TextureLevel mLevel;
	uint8* mData;
	uint32 mBytesPerLine;
	uint32 mBytesPerSlice;
	uint32 mSize;

	operator void* () const { return mData; }
};

class SR_TextureResource : public SR_Resource
{
public:
	const SR_TextureResourceProperties& GetProperties() const;

	virtual void UpdatePixels(const SR_PixelData* aData, uint32 aDataCount);

protected:
	SR_TextureResource(const SR_TextureResourceProperties& aProperties);
	virtual ~SR_TextureResource();

	SR_TextureResourceProperties mProperties;
};

inline uint32 SR_GetNumSubresources(const SR_TextureResourceProperties& aProperties) { return aProperties.mNumMips * SC_Max(aProperties.mSize.z, 1U) * (aProperties.mIsCubeMap ? 6 : 1); }
inline uint32 SC_GetMaximumNumMips(const SC_IntVector& aResolution)
{
	return 1 + static_cast<uint32>(SC_Math::Floor(SC_Math::Log2(static_cast<float>(SC_Max(aResolution.x, aResolution.y, aResolution.z)))));
}

