#include "SR_DDSTextureLoader.h"

#include "Defines/SR_DDS.h"
#include <fstream>

static bool ReadTextureData(std::ifstream& aFile, SR_TextureData& /*aOutData*/, uint8* aDstBuffer, uint32 aSize, uint32 /*aOffset*/)
{
	assert(aFile.is_open());

	//aFile.seekg(aOffset, std::ios::beg);
	aFile.read((char*)aDstBuffer, aSize);
	return true;
}


static SR_Format GetFormatFromDDSPF(const DDS_PIXELFORMAT& ddpf)
{
#define ISBITMASK(r,g,b,a) (ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a)

	if (ddpf.flags & DDS_RGB)
	{
		switch (ddpf.RGBBitCount)
		{
		case 32:
			if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
				return SR_Format::RGBA8_UNORM;
			if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000))
				return SR_Format::RGBA8_UNORM;
			if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
				return SR_Format::BGRA8_UNORM;
			if (ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
				return SR_Format::RG16_UNORM;
			if (ISBITMASK(0xffffffff, 0x00000000, 0x00000000, 0x00000000))
				return SR_Format::R32_FLOAT;
			if (ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
				return SR_Format::RGB10A2_UNORM;
			break;

		case 24:
			// No 24bpp DXGI formats
			break;

		case 16:
			if (ISBITMASK(0x00ff, 0, 0, 0xff00))
				return SR_Format::RG8_UNORM;
			if (ISBITMASK(0xffff, 0, 0, 0))
				return SR_Format::R16_UNORM;
			break;

		case 8:
			if (ISBITMASK(0xff, 0, 0, 0))
				return SR_Format::R8_UNORM;
			break;
		}
	}
	else if (ddpf.flags & DDS_LUMINANCE)
	{
		if (8 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x00000000))
				return SR_Format::R8_UNORM;
		}

		if (16 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x0000ffff, 0x00000000, 0x00000000, 0x00000000))
				return SR_Format::R16_UNORM;
			if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
				return SR_Format::RG8_UNORM;
		}
	}
	//else if (ddpf.flags & DDS_ALPHA)
	//{
	//}
	else if (ddpf.flags & DDS_BUMPDUDV)
	{
		switch (ddpf.RGBBitCount)
		{
		case 32:
			if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
				return SR_Format::RGBA8_SNORM;
			if (ISBITMASK(0x0000ffff, 0xffff0000, 0, 0))
				return SR_Format::RG16_SNORM;

			break;

		case 16:
			if (ISBITMASK(0x00ff, 0xff00, 0, 0))
				return SR_Format::RG8_SNORM;
			break;
		}
	}
	else if (ddpf.flags & DDS_FOURCC)
	{
		if (MAKEFOURCC('D', 'X', 'T', '1') == ddpf.fourCC)
			return SR_Format::BC1_UNORM;
		if (MAKEFOURCC('D', 'X', 'T', '2') == ddpf.fourCC)
			return SR_Format::BC2_UNORM;
		if (MAKEFOURCC('D', 'X', 'T', '3') == ddpf.fourCC)
			return SR_Format::BC2_UNORM;
		if (MAKEFOURCC('D', 'X', 'T', '4') == ddpf.fourCC)
			return SR_Format::BC3_UNORM;
		if (MAKEFOURCC('D', 'X', 'T', '5') == ddpf.fourCC)
			return SR_Format::BC3_UNORM;
		if (MAKEFOURCC('A', 'T', 'I', '1') == ddpf.fourCC)
			return SR_Format::BC4_UNORM;
		if (MAKEFOURCC('B', 'C', '4', 'U') == ddpf.fourCC)
			return SR_Format::BC4_UNORM;
		if (MAKEFOURCC('B', 'C', '4', 'S') == ddpf.fourCC)
			return SR_Format::BC4_SNORM;
		if (MAKEFOURCC('A', 'T', 'I', '2') == ddpf.fourCC)
			return SR_Format::BC5_UNORM;
		if (MAKEFOURCC('B', 'C', '5', 'U') == ddpf.fourCC)
			return SR_Format::BC5_UNORM;
		if (MAKEFOURCC('B', 'C', '5', 'S') == ddpf.fourCC)
			return SR_Format::BC5_SNORM;

		switch (ddpf.fourCC)
		{
		case 36: // D3DFMT_A16B16G16R16
			return SR_Format::RGBA16_UNORM;
		case 110: // D3DFMT_Q16W16V16U16
			return SR_Format::RGBA16_SNORM;
		case 111: // D3DFMT_R16F
			return SR_Format::R16_FLOAT;
		case 112: // D3DFMT_G16R16F
			return SR_Format::RG16_FLOAT;
		case 113: // D3DFMT_A16B16G16R16F
			return SR_Format::RGBA16_FLOAT;
		case 114: // D3DFMT_R32F
			return SR_Format::R32_FLOAT;
		case 115: // D3DFMT_G32R32F
			return SR_Format::RG32_FLOAT;
		case 116: // D3DFMT_A32B32G32R32F
			return SR_Format::RGBA32_FLOAT;
		}
	}

	return SR_Format::UNKNOWN;
#undef ISBITMASK
}

bool DecodeHeader(std::ifstream& aFile, SR_TextureData& aOutData)
{
	char magic[4] = {};
	aFile.read(magic, sizeof(magic));
	if (memcmp(magic, "DDS ", sizeof(magic)) != 0)
	{
		// invalid dds file
		return false;
	}

	DDS_HEADER header = {};
	aFile.read((char*)(&header), sizeof(header));

	// Verify header to validate DDS file
	if (header.size != sizeof(DDS_HEADER) || header.ddspf.size != sizeof(DDS_PIXELFORMAT))
	{
		// invalid DDS Header
		return false;
	}

	// Check for DX10 extension
	DDS_HEADER_DXT10 dxt10header = {};
	bool dxt10 = false;
	if ((header.ddspf.flags & DDS_FOURCC) && (MAKEFOURCC('D', 'X', '1', '0') == header.ddspf.fourCC))
	{
		//// Must be long enough for both headers and magic value
		//if (fileSize < (sizeof(DDS_HEADER) + sizeof(uint32) + sizeof(DDS_HEADER_DXT10)))
		//{
		//	//too small to be a valid DX10 dds file
		//	return false;
		//}

		dxt10 = true;
		aFile.read((char*)(&dxt10header), sizeof(dxt10header));
	}


	aOutData.mProperties.mSize.x = header.width;
	aOutData.mProperties.mSize.y = header.height;
	aOutData.mProperties.mType = SR_ResourceType::Texture2D;
	if (dxt10 && dxt10header.dxgiFormat)
		aOutData.mProperties.mFormat = SR_D3D12ConvertFormat(dxt10header.dxgiFormat);
	else
		aOutData.mProperties.mFormat = GetFormatFromDDSPF(header.ddspf);
	aOutData.mStoredFormat = aOutData.mProperties.mFormat;

	if (aOutData.mProperties.mFormat == SR_Format::UNKNOWN)
		return false;

	if (header.flags & DDS_HEADER_FLAGS_VOLUME)
	{
		aOutData.mProperties.mSize.z = header.depth;
		if (aOutData.mProperties.mSize.z > 1)
			aOutData.mProperties.mType = SR_ResourceType::Texture3D;
	}

	if (header.flags & DDS_HEADER_FLAGS_MIPMAP)
	{
		uint32 minSide = SC_Min(aOutData.mProperties.mSize.x, aOutData.mProperties.mSize.y);
		uint32 pow2Size = SC_GetNextPow2(minSide);
		if (minSide == pow2Size || (minSide & ((1 << (header.mipMapCount + 1)) - 1)) == 0)
			aOutData.mProperties.mNumMips = uint8(SC_Max(1, int(header.mipMapCount)));
		else
			aOutData.mProperties.mNumMips = 1;
	}

	if (header.caps & DDS_SURFACE_FLAGS_CUBEMAP)
	{
		if (header.caps2 & DDS_CUBEMAP)
		{
			if ((header.caps2 & DDS_CUBEMAP_ALLFACES) != DDS_CUBEMAP_ALLFACES)
			{
				// contains an incomplete cube map texture
				return false;
			}

			aOutData.mProperties.mType = SR_ResourceType::Texture2D;
			aOutData.mProperties.mIsCubeMap = true;
		}
	}

	if (dxt10)
	{
		if (dxt10header.miscFlag & DDS_RESOURCE_MISC_TEXTURECUBE)
		{
			aOutData.mProperties.mType = SR_ResourceType::Texture2D;
			aOutData.mProperties.mIsCubeMap = true;
		}

		if (dxt10header.arraySize != 1)
		{
			assert(dxt10header.resourceDimension == DDS_DIMENSION_TEXTURE2D);
			aOutData.mProperties.mType = SR_ResourceType::Texture2D;
			aOutData.mProperties.mSize.z = dxt10header.arraySize;
		}
	}

	aOutData.mDataOffset = sizeof(DDS_HEADER) + sizeof(uint32) + (dxt10 ? sizeof(dxt10header) : 0);
	aOutData.mAvailableMipCount = aOutData.mProperties.mNumMips;

	return true;
}


bool LoadPixelData(std::ifstream& aFile, SR_TextureData& aOutData, uint32 aStartMip, uint32 aEndMip, uint32 aMipOffset)
{
	const SR_TextureResourceProperties& props = aOutData.mProperties;
	SR_TextureSizes sizes(props);

	const SR_TextureResourceProperties croppedProps(aOutData.mProperties, aStartMip);
	SR_TextureSizes croppedSizes(croppedProps);

	uint32 numTextures = sizes.mNumTextures;
	uint8* buffer = new uint8[croppedSizes.mByteSize];
	const uint8* bufferEnd = buffer + croppedSizes.mByteSize;
	aOutData.mBuffers.Add(buffer);

	for (uint32 i = 0; i < numTextures; ++i)
	{
		SR_TexturePixelRange range;
		if (props.mIsCubeMap)
			range.mLevel.mFace = SR_CubeMapFace(i % 6);
		else
			range.mLevel.mArrayIndex = i / sizes.mNumTexturesPerArrayIndex;

		int filePos = 0;
		uint32 totalReadSize = 0;
		uint32 startReadPos = 0;
		for (uint32 mip = aStartMip; mip < aEndMip; ++mip)
		{
			range.mLevel.mMipLevel = mip;
			SR_TexturePixelRange curRange(mip);
			curRange.mDepthSize = SC_Max(1, props.mSize.z >> mip);
			SR_TextureResourceProperties mipProps(props, curRange);
			SR_TextureSizes mipSizes(mipProps);

			int mipFilePos = sizes.GetDDSDataOffset(range) + aOutData.mDataOffset;

			if (mip == aStartMip)
				startReadPos = mipFilePos;
			else
				assert((mipFilePos == filePos) && "Invalid mip offset");

			filePos = mipFilePos + mipSizes.mMips[0].mBytesPerTexture;
			totalReadSize += mipSizes.mMips[0].mBytesPerTexture;
		}

		if (!ReadTextureData(aFile, aOutData, buffer, totalReadSize, startReadPos))
		{
			// failed to read texture data
			return false;
		}

		for (uint32 mip = aStartMip; mip < aEndMip; ++mip)
		{
			range.mLevel.mMipLevel = mip;
			SR_TexturePixelRange curRange(mip);
			curRange.mDepthSize = SC_Max(1, props.mSize.z >> mip);
			SR_TextureResourceProperties mipProps(props, curRange);
			SR_TextureSizes mipSizes(mipProps);

			if (aOutData.mStoredFormat != aOutData.mProperties.mFormat)
			{
				//ConvertTextureData(buffer, mipSizes.mByteSize, aOutData);
			}

			const SR_TextureSizes::Mip& mipData = mipSizes.mMips[0];
			SR_PixelData& data = aOutData.mPixelData.Add();
			data.mData = buffer;
			data.mBytesPerLine = mipData.mBytesPerLine;
			data.mBytesPerSlice = mipData.mBytesPerSlice;
			data.mSize = mipData.mBytesPerTexture;
			data.mLevel = range.mLevel;
			data.mLevel.mMipLevel -= aMipOffset;

			buffer += mipData.mBytesPerTexture;
			assert(buffer <= bufferEnd);
			(void)bufferEnd;
		}
	}

	return true;
}

bool SR_LoadTextureDataDDS(const SC_FilePath& aFilePath, SR_TextureData& aOutData)
{
	std::ifstream file(aFilePath.GetStr());
	if (!file.is_open())
		return false;

	if (DecodeHeader(file, aOutData))
	{
		return LoadPixelData(file, aOutData, 0, aOutData.mProperties.mNumMips, 0);
	}

	return true;
}