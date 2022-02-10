#pragma once

enum class SR_Format
{
	UNKNOWN,

	RGBA32_TYPELESS,
	RGBA32_FLOAT,
	RGBA32_UINT,
	RGBA32_SINT,

	RGB32_TYPELESS,
	RGB32_FLOAT,
	RGB32_UINT,
	RGB32_SINT,

	RGBA16_TYPELESS,
	RGBA16_FLOAT,
	RGBA16_UNORM,
	RGBA16_UINT,
	RGBA16_SNORM,
	RGBA16_SINT,

	RGBA8_TYPELESS,
	RGBA8_UNORM,
	RGBA8_UNORM_SRGB,
	RGBA8_UINT,
	RGBA8_SNORM,
	RGBA8_SINT,

	BGRA8_UNORM,

	RGB10A2_TYPELESS,
	RGB10A2_UNORM,
	RGB10A2_UINT,

	RG11B10_FLOAT,

	RG32_TYPELESS,
	RG32_FLOAT,
	RG32_UINT,
	RG32_SINT,

	D32_FLOAT_S8X24_UINT,
	R32_FLOAT_X8X24_TYPELESS,
	X32_TYPELESS_G8X24_UINT,

	RG16_TYPELESS,
	RG16_FLOAT,
	RG16_UNORM,
	RG16_UINT,
	RG16_SNORM,
	RG16_SINT,

	RG8_TYPELESS,
	RG8_UNORM,
	RG8_UINT,
	RG8_SNORM,
	RG8_SINT,

	R32_TYPELESS,
	R32_FLOAT,
	D32_FLOAT,
	R32_UINT,
	R32_SINT,

	R16_TYPELESS,
	R16_FLOAT,
	R16_UNORM,
	D16_UNORM,
	R16_UINT,
	R16_SNORM,
	R16_SINT,

	R8_TYPELESS,
	R8_UNORM,
	R8_UINT,
	R8_SNORM,
	R8_SINT,

	BC1_UNORM, // DXT1
	BC2_UNORM, // DXT3
	BC3_UNORM, // DXT5
	BC4_UNORM,
	BC4_SNORM,
	BC5_UNORM,
	BC5_SNORM,
	BC7,
	BC7_SRGB,

	R1_BOOL,
};

inline uint32 SR_GetFormatBlockSize(const SR_Format& aFormat)
{ 
	switch (aFormat)
	{
	case SR_Format::BC1_UNORM:
	case SR_Format::BC2_UNORM:
	case SR_Format::BC3_UNORM:
	case SR_Format::BC4_UNORM:
	case SR_Format::BC4_SNORM:
	case SR_Format::BC5_UNORM:
	case SR_Format::BC5_SNORM:
	case SR_Format::BC7:
	case SR_Format::BC7_SRGB:
		return 4;
	}

	return 1;
}

inline uint32 SR_GetFormatBitsPerPixel(const SR_Format& aFormat)
{

	switch (aFormat)
	{
	case SR_Format::RGBA32_TYPELESS:
	case SR_Format::RGBA32_FLOAT:
	case SR_Format::RGBA32_UINT:
	case SR_Format::RGBA32_SINT:
		return 128;

	case SR_Format::RGB32_TYPELESS:
	case SR_Format::RGB32_FLOAT:
	case SR_Format::RGB32_UINT:
	case SR_Format::RGB32_SINT:
		return 96;

	case SR_Format::RGBA16_TYPELESS:
	case SR_Format::RGBA16_FLOAT:
	case SR_Format::RGBA16_UINT:
	case SR_Format::RGBA16_SINT:
	case SR_Format::RG32_TYPELESS:
	case SR_Format::RG32_FLOAT:
	case SR_Format::RG32_UINT:
	case SR_Format::RG32_SINT:
	case SR_Format::D32_FLOAT_S8X24_UINT:
	case SR_Format::R32_FLOAT_X8X24_TYPELESS:
	case SR_Format::X32_TYPELESS_G8X24_UINT:
		return 64;

	case SR_Format::RGBA8_TYPELESS:
	case SR_Format::RGBA8_UNORM:
	case SR_Format::RGBA8_UNORM_SRGB:
	case SR_Format::RGBA8_UINT:
	case SR_Format::RGBA8_SNORM:
	case SR_Format::RGBA8_SINT:
	case SR_Format::BGRA8_UNORM:
	case SR_Format::RGB10A2_TYPELESS:
	case SR_Format::RGB10A2_UNORM:
	case SR_Format::RGB10A2_UINT:
	case SR_Format::RG11B10_FLOAT:
	case SR_Format::RG16_TYPELESS:
	case SR_Format::RG16_FLOAT:
	case SR_Format::RG16_UNORM:
	case SR_Format::RG16_UINT:
	case SR_Format::RG16_SNORM:
	case SR_Format::RG16_SINT:
	case SR_Format::R32_TYPELESS:
	case SR_Format::R32_FLOAT:
	case SR_Format::D32_FLOAT:
	case SR_Format::R32_UINT:
	case SR_Format::R32_SINT:
		return 32;

	case SR_Format::RG8_TYPELESS:
	case SR_Format::RG8_UNORM:
	case SR_Format::RG8_UINT:
	case SR_Format::RG8_SNORM:
	case SR_Format::R16_TYPELESS:
	case SR_Format::R16_FLOAT:
	case SR_Format::R16_UNORM:
	case SR_Format::D16_UNORM:
	case SR_Format::R16_UINT:
	case SR_Format::R16_SNORM:
	case SR_Format::R16_SINT:
		return 16;

	case SR_Format::R8_TYPELESS:
	case SR_Format::R8_UNORM:
	case SR_Format::R8_UINT:
	case SR_Format::R8_SNORM:
	case SR_Format::R8_SINT:
	case SR_Format::BC2_UNORM:
	case SR_Format::BC3_UNORM:
	case SR_Format::BC5_UNORM:
	case SR_Format::BC5_SNORM:
	case SR_Format::BC7:
	case SR_Format::BC7_SRGB:
	case SR_Format::R1_BOOL:
		return 8;

	case SR_Format::BC1_UNORM:
	case SR_Format::BC4_UNORM:
	case SR_Format::BC4_SNORM:
		return 4;

	default:
		return 0;
	}
}