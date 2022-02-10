#pragma once
#include "RenderCore/Interface/SR_TextureResource.h"

struct SR_TextureData
{
	SR_TextureResourceProperties mProperties;
	SC_Array<uint8*> mBuffers;
	SC_Array<SR_PixelData> mPixelData;

	SR_Format mStoredFormat;
	uint32 mDataOffset;
	uint16 mAvailableMipCount;

	SR_TextureData() : mStoredFormat(SR_Format::UNKNOWN), mDataOffset(0), mAvailableMipCount((uint16)-1) {}
	~SR_TextureData() { for (uint32 i = 0; i < mBuffers.Count(); ++i) { delete mBuffers[i]; } mBuffers.RemoveAll(); }
};

struct SR_TextureSizes
{
	SR_TextureSizes() { SC_ZeroMemory(this, sizeof(SR_TextureSizes)); }
	explicit SR_TextureSizes(const SR_TextureResourceProperties& aProperties);

	uint32 GetDDSDataOffset(const SR_TextureLevel& aLevel);
	uint32 GetDDSDataOffset(const SR_TexturePixelRange& aRange);

	struct Mip
	{
		uint32 mByteSize;
		uint32 mBytesPerArrayIndex;
		uint32 mBytesPerTexture;
		uint32 mBytesPerSlice;
		uint32 mBytesPerLine;
		uint32 mMipOffset;
		SC_IntVector2 mBlockSliceSize;
	};

	Mip mMips[SR_MaxMipCount];

	uint32 mByteSize;
	uint32 mBlockSize;
	uint32 mBitsPerPixel;
	uint32 mMipChainBytesPerTexture;

	uint32 mNumTextures;
	uint32 mNumTextureLevels;
	uint32 mNumTexturesPerArrayIndex;
};

bool SR_LoadTextureData(const char* aFilePath, SR_TextureData& aOutData);