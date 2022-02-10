#pragma once
#include "Platform/Types/SC_TypeDefines.h"
#include "RenderCore/Defines/SR_Format.h"

enum class SR_VertexAttribute
{
	Position,
	Normal,
	Tangent,
	Bitangent,
	UV0,
	UV1,
	Color0,
	Color1,
	COUNT
};

//static const char* SR_GetTypeFromVertexFormat(const SR_Format& aFormat)
//{
//	switch (aFormat)
//	{
//	case SR_Format::RGBA32_FLOAT:
//		return "float4";
//	case SR_Format::RGB32_FLOAT:
//		return "float3";
//	case SR_Format::RG32_FLOAT:
//		return "float2";
//	case SR_Format::R32_FLOAT:
//		return "float";
//	}
//
//	return "";
//}

struct SR_VertexLayout
{
	SR_VertexLayout()
	{
		SC_Fill(mAttributeFormats, static_cast<uint32>(SR_VertexAttribute::COUNT), SR_Format::UNKNOWN);
	}

	uint32 GetVertexStrideSize() const
	{
		uint32 stride = 0;

		for (const SR_Format& format : mAttributeFormats)
		{
			uint32 bytesPerAttribute = SR_GetFormatBitsPerPixel(format) / 8;
			stride += bytesPerAttribute;
		}

		return stride;
	}

	void SetAttribute(const SR_VertexAttribute& aAttribute, const SR_Format aFormat = SR_Format::UNKNOWN)
	{
		mAttributeFormats[static_cast<uint32>(aAttribute)] = aFormat;
	}

	bool HasAttribute(const SR_VertexAttribute& aAttribute) const
	{
		return GetAttributeFormat(aAttribute) != SR_Format::UNKNOWN;
	}

	const SR_Format& GetAttributeFormat(const SR_VertexAttribute& aAttribute) const
	{
		return mAttributeFormats[static_cast<uint32>(aAttribute)];
	}

	bool operator==(const SR_VertexLayout& aOther) const 
	{
		for (uint32 i = 0; i < static_cast<uint32>(SR_VertexAttribute::COUNT); ++i)
		{
			if (mAttributeFormats[i] != aOther.mAttributeFormats[i])
				return false;
		}

		return true;
	}

	SR_Format mAttributeFormats[static_cast<uint32>(SR_VertexAttribute::COUNT)];
};

namespace std
{
	template <>
	struct hash<SR_VertexLayout>
	{
		size_t operator()(const SR_VertexLayout& aOther) const
		{
			size_t res = 17;
			for (uint32 i = 0; i < static_cast<uint32>(SR_VertexAttribute::COUNT); ++i)
			{
				res = res * 31 + hash<uint32>()(static_cast<uint32>(aOther.mAttributeFormats[i]));
			}
			return res;
		}
	};
}