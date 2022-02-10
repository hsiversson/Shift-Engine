#include "SR_TGATextureLoader.h"
#include <fstream>

#define ENABLE_RLE (1)

#pragma pack (push,1)
struct TGAheader
{
	uint8 mImageIDLength;
	uint8 mColorMapType;
	uint8 mImageType;
	//begin ColorMapSpecification field
	uint16 mFirstEntryIndex;
	uint16 mColorMapEntryCount;
	uint8  mColorMapEntrySize;
	//end ColorMapSpecification field
	//begin ImageSpecification field
	uint16 mOriginX;
	uint16 mOriginY;
	uint16 mWidth;
	uint16 mHeight;
	uint8 mPixelDepth;
	uint8 mImageDescriptor;
	//end ImageDescriptor bit-field
	//end ImageSpecification field
};
#pragma pack(pop)

enum TGAType
{
	TGAType_Unknown = 0,
	TGAType_Palettized,
	TGAType_UncompressedRGB,
	TGAType_UncompressedGrayscale,
	TGAType_RLECompressedRGB = 8,
};

#if ENABLE_RLE
static bool LoadRLECompressed(std::ifstream& aFile, SR_PixelData& pixelData, uint32 aBpp)
{
	uint8* writeStart = pixelData.mData;
	if (aBpp > sizeof(uint32))
		return false;

	uint8* currentWritePos = writeStart;
	while (currentWritePos < writeStart + pixelData.mSize)
	{
		uint8 cur;
		if (!aFile.read((char*)&cur, sizeof(uint8)))
			return false;

		if (cur & 0x80) // run length chunk
		{
			const uint32 length = cur - 127;	// lower 7 bits of cur + one
			uint32 pixel;
			if (!aFile.read((char*)&pixel, aBpp))
				return false;

			for (uint32 i = 0; i < length; ++i)
			{
				SC_Memcpy(currentWritePos, &pixel, aBpp);
				currentWritePos += aBpp;
			}
		}
		else // raw chunk
		{
			const uint32 length = (cur + 1) * aBpp;	// lower 7 bits of cur + one
			if (!aFile.read((char*)currentWritePos, length))
				return false;
			currentWritePos += length;
		}
	}

	return true;
}
#endif

bool SR_LoadTextureDataTGA(const SC_FilePath& aFilePath, SR_TextureData& aOutData)
{
	uint32 bpp = 0;
	uint64 fileSize = 0;

	bool flipX = false;
	bool flipY = false;

	std::ifstream file(aFilePath.GetAbsolutePath(), std::ios::binary);
	if (!file.is_open())
		return false;

	fileSize = SC_File::GetFileSize(aFilePath);

	TGAheader header;
	if (fileSize < sizeof(TGAheader))
	{
		// file is not big enough to contain the header which makes it invalid
		return false;
	}
	file.read((char*)&header, sizeof(TGAheader));

	flipY = (header.mImageDescriptor & 32) == 0;
	flipX = (header.mImageDescriptor & 16) != 0;

	if (header.mImageType == TGAType_Palettized || header.mImageType == TGAType_UncompressedGrayscale)
	{
		if (header.mPixelDepth != 8)
		{
			return false;
		}
		bpp = header.mPixelDepth;
	}
	else if (header.mImageType == TGAType_UncompressedRGB)
	{
		if (header.mPixelDepth != 16 && header.mPixelDepth != 24 && header.mPixelDepth != 32)
		{
			return false;
		}
		bpp = header.mPixelDepth;
	}
#if ENABLE_RLE
	else if (header.mImageType & TGAType_RLECompressedRGB)
	{
		bpp = header.mPixelDepth;
	}
#endif
	else
		return false;

	SR_TextureChannelOrder channels;
	if (bpp == 32)
		channels = SR_CHANNELORDER_BGRA;
	else if (bpp == 24)
		channels = SR_CHANNELORDER_BGR;
	else if (bpp == 16)
		channels = SR_CHANNELORDER_RG;
	else if (bpp == 8)
		channels = SR_CHANNELORDER_R;
	else
		return false;

	uint32 resolution = header.mWidth * header.mHeight;
	uint32 bytesPerPixel = bpp / 8;
	SR_PixelData& pixelData = aOutData.mPixelData.Add();
	pixelData.mSize = resolution * bytesPerPixel;
	pixelData.mBytesPerLine = header.mWidth * bytesPerPixel;
	pixelData.mBytesPerSlice = header.mHeight * bytesPerPixel;
	pixelData.mData = new uint8[pixelData.mSize];

	uint32 paletteSize = 0;
	if (header.mImageType == TGAType_Palettized)
		paletteSize = (header.mColorMapEntryCount * header.mColorMapEntrySize) / 8;

	const uint32 imageDataBegin = uint32(sizeof(TGAheader) + header.mImageIDLength + paletteSize);
	file.seekg(imageDataBegin);

#if ENABLE_RLE
	if (header.mImageType & 8) // RLE RGB
	{
		if (!LoadRLECompressed(file, pixelData, bpp))
		{
			return false;
		}
	}
	else
#endif
	{
		if (fileSize < imageDataBegin + pixelData.mSize)
		{
			// error: incomplete TGA file
			return false;
		}

		if (!file.read((char*)pixelData.mData, pixelData.mSize))
		{
			// error: incomplete TGA file
			return false;
		}
	}

	if (flipY)
	{
		const int bytesPerLine = header.mWidth * bytesPerPixel;
		uint8* p1 = pixelData.mData;
		uint8* p2 = p1 + (header.mHeight - 1) * bytesPerLine;

		while (p1 < p2)
		{
			uint32 x = bytesPerLine;

			// do quad words
			for (; x > 3; x -= 4)
			{
				// Swap 4 bytes
				uint32 ti = *((uint32*)p1);
				*((uint32*)p1) = *((uint32*)p2);
				*((uint32*)p2) = ti;

				p1 += 4;
				p2 += 4;
			}

			// do bytes
			for (; x > 0; --x)
			{
				// Swap one byte
				uint8 t = *p1;
				*p1 = *p2;
				*p2 = t;

				p1++;
				p2++;
			}

			p2 -= 2 * bytesPerLine;
		}
	}

	if (flipX)
	{

	}

	SR_Format format = SR_Format::UNKNOWN;
	if (channels == SR_CHANNELORDER_BGRA)
	{
		uint8* currentPos = pixelData.mData;
		for (uint32 pixel = 0; pixel < resolution; ++pixel) // Convert BGRA -> RGBA
		{
			uint8 data[3] = { currentPos[0], currentPos[1], currentPos[2] };
			currentPos[0] = data[2];
			currentPos[1] = data[1];
			currentPos[2] = data[0];
			currentPos += 4; // also skip alpha channel
		}
		format = SR_Format::RGBA8_UNORM;
	}
	else if (channels == SR_CHANNELORDER_BGR)
	{
		// convert into 32bpp and use RGBA8_UNORM format.
		uint32 expandedSize = resolution * 4; // 32bpp gives 4 bytes per pixel
		uint8* expandedData = new uint8[expandedSize];
		uint8* currentPos = pixelData.mData;
		uint8* currentPosExpanded = expandedData;
		for (uint32 pixel = 0; pixel < resolution; ++pixel) // Convert BGRA -> RGBA + expand
		{
			uint8 data[3] = { currentPos[0], currentPos[1], currentPos[2] };
			currentPosExpanded[0] = data[2];
			currentPosExpanded[1] = data[1];
			currentPosExpanded[2] = data[0];
			currentPosExpanded[3] = 255;
			currentPos += 3;
			currentPosExpanded += 4;
		}
		format = SR_Format::RGBA8_UNORM;
		delete[] pixelData.mData;
		pixelData.mData = expandedData;
		pixelData.mSize = header.mWidth * header.mHeight * 4;
		pixelData.mBytesPerLine = header.mWidth * 4;
		pixelData.mBytesPerSlice = header.mHeight * 4;
	}
	else if (channels == SR_CHANNELORDER_RG)
		format = SR_Format::RG8_UNORM;
	else if (channels == SR_CHANNELORDER_R)
		format = SR_Format::R8_UNORM;

	aOutData.mProperties.mSize.x = header.mWidth;
	aOutData.mProperties.mSize.y = header.mHeight;
	aOutData.mProperties.mType = SR_ResourceType::Texture2D;
	aOutData.mProperties.mFormat = format;

	return true;
}
