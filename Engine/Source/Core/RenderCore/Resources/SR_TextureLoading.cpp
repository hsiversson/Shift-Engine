#include "SR_TextureLoading.h"
#include "SR_DDSTextureLoader.h"
#include "SR_TGATextureLoader.h"

bool SR_LoadTextureDataDefault(const SC_FilePath& /*aFilePath*/, SR_TextureData& /*aOutData*/)
{

	return true;
}

bool SR_LoadTextureData(const char* aFilePath, SR_TextureData& aOutData)
{
	SC_FilePath path(aFilePath);
	std::string extension = path.GetFileExtension();

	if (extension == ".dds")
		return SR_LoadTextureDataDDS(path, aOutData);
	else if (extension == ".tga")
		return SR_LoadTextureDataTGA(path, aOutData);
	else
		return SR_LoadTextureDataDefault(path, aOutData);
}

SR_TextureSizes::SR_TextureSizes(const SR_TextureResourceProperties& aProperties)
{
	SC_ASSERT(aProperties.mFormat != SR_Format::UNKNOWN, "Format cannot be unknown!");

	uint32 arraySize = SC_Max(aProperties.mArraySize, 1U);

	mNumTexturesPerArrayIndex = aProperties.mIsCubeMap ? 6 : 1;
	mNumTextures = mNumTexturesPerArrayIndex * arraySize;
	mNumTextureLevels = mNumTextures * aProperties.mNumMips;

	uint32 blockSize = SR_GetFormatBlockSize(aProperties.mFormat);
	uint32 bitsPerPixel = SR_GetFormatBitsPerPixel(aProperties.mFormat);
	mBlockSize = blockSize;
	mBitsPerPixel = bitsPerPixel;

	mMipChainBytesPerTexture = 0;
	assert(aProperties.mNumMips <= SR_MaxMipCount);
	for (uint32 i = 0; i < aProperties.mNumMips; ++i)
	{
		uint32 width = SC_Max(aProperties.mSize.x >> i, 1);
		uint32 height = SC_Max(aProperties.mSize.y >> i, 1);
		uint32 depth = SC_Max(aProperties.mSize.z >> i, 1);
		width = ((width + blockSize - 1) / blockSize) * blockSize;
		height = ((height + blockSize - 1) / blockSize) * blockSize;

		uint32 bytesPerLine = width * bitsPerPixel / 8;
		uint32 bytesPerSlice = bytesPerLine * height;
		uint32 bytesPerTexture = bytesPerSlice * depth;
		uint32 bytesPerArrayIndex = bytesPerTexture * mNumTexturesPerArrayIndex;
		uint32 byteSize = bytesPerArrayIndex * arraySize;

		mMips[i].mBlockSliceSize.x = width;
		mMips[i].mBlockSliceSize.y = height;
		mMips[i].mBytesPerLine = bytesPerLine;
		mMips[i].mBytesPerSlice = bytesPerSlice;
		mMips[i].mBytesPerTexture = bytesPerTexture;
		mMips[i].mBytesPerArrayIndex = bytesPerArrayIndex;
		mMips[i].mByteSize = byteSize;
		mMips[i].mMipOffset = mMipChainBytesPerTexture;

		mMipChainBytesPerTexture += bytesPerTexture;
	}
	SC_ZeroMemory(mMips + aProperties.mNumMips, sizeof(Mip) * (SR_MaxMipCount - aProperties.mNumMips));

	mByteSize = mMipChainBytesPerTexture * mNumTextures;
}

uint32 SR_TextureSizes::GetDDSDataOffset(const SR_TextureLevel& aLevel)
{
	uint32 tex = mNumTexturesPerArrayIndex * aLevel.mArrayIndex;
	tex += aLevel.mFace;
	uint32 texOffset = tex * mMipChainBytesPerTexture;

	assert(aLevel.mMipLevel < SR_MaxMipCount);
	const Mip& mip = mMips[aLevel.mMipLevel];
	return texOffset + mip.mMipOffset;
}

uint32 SR_TextureSizes::GetDDSDataOffset(const SR_TexturePixelRange& aRange)
{
	const Mip& mip = mMips[aRange.mLevel.mMipLevel];
	uint32 texLevelOffset = GetDDSDataOffset(aRange.mLevel);
	uint32 depthOffset = mip.mBytesPerSlice * aRange.mDepthIndex;
	return texLevelOffset + depthOffset;
}
