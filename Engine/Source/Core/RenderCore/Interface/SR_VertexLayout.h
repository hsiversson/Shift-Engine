#pragma once
#include "Platform/Types/SC_TypeDefines.h"
#include "RenderCore/Defines/SR_Format.h"

enum class SR_VertexAttribute
{
	Position,
	Normal,
	Tangent,
	Bitangent,
	UV,
	Color,
	BoneId,
	BoneWeight,
	COUNT
};

struct SR_VertexAttributeData
{
	SR_VertexAttributeData() : mAttributeId(SR_VertexAttribute::COUNT), mFormat(SR_Format::UNKNOWN), mAttributeIndex(0) {}
	SR_VertexAttributeData(const SR_VertexAttribute& aAttribute, const SR_Format& aFormat, uint32 aAttributeIndex = 0) : mAttributeId(aAttribute), mFormat(aFormat), mAttributeIndex(aAttributeIndex) {}

	SR_VertexAttribute mAttributeId;
	SR_Format mFormat;
	uint32 mAttributeIndex;
};

struct SR_VertexLayout
{
	SR_VertexLayout() {}

	uint32 GetVertexStrideSize() const
	{
		uint32 stride = 0;

		for (const SR_VertexAttributeData& attribute : mAttributes)
		{
			uint32 bytesPerAttribute = SR_GetFormatBitsPerPixel(attribute.mFormat) / 8;
			stride += bytesPerAttribute;
		}

		return stride;
	}

	void SetAttribute(const SR_VertexAttribute& aAttribute, const SR_Format aFormat = SR_Format::UNKNOWN, uint32 aIndex = 0)
	{
		for (SR_VertexAttributeData& attrib : mAttributes)
		{
			if (attrib.mAttributeId == aAttribute && attrib.mAttributeIndex == aIndex)
			{
				attrib.mFormat = aFormat;
				return;
			}
		}

		mAttributes.Add(SR_VertexAttributeData(aAttribute, aFormat, aIndex));
	}

	bool HasAttribute(const SR_VertexAttribute& aAttribute, uint32 aIndex = 0) const
	{
		for (const SR_VertexAttributeData& attrib : mAttributes)
		{
			if (attrib.mAttributeId == aAttribute && attrib.mAttributeIndex == aIndex)
				return true;
		}

		return false;
	}

	SR_Format GetAttributeFormat(const SR_VertexAttribute& aAttribute, uint32 aIndex = 0) const
	{
		for (const SR_VertexAttributeData& attrib : mAttributes)
		{
			if (attrib.mAttributeId == aAttribute && attrib.mAttributeIndex == aIndex)
				return attrib.mFormat;
		}

		return SR_Format::UNKNOWN;
	}

	uint32 GetAttributeByteOffset(const SR_VertexAttribute& aAttribute, uint32 aIndex = 0) const
	{
		uint32 offset = 0;
		for (const SR_VertexAttributeData& attrib : mAttributes)
		{
			if (attrib.mAttributeId == aAttribute && attrib.mAttributeIndex == aIndex)
				break;

			uint32 attributeBytes = SR_GetFormatBitsPerPixel(attrib.mFormat) / 8;
			offset += attributeBytes;
		}
		return offset;
	}

	bool operator==(const SR_VertexLayout& aOther) const 
	{
		if (mAttributes.Count() != aOther.mAttributes.Count())
			return false;

		for (uint32 i = 0; i < mAttributes.Count(); ++i)
		{
			if (mAttributes[i].mAttributeId != aOther.mAttributes[i].mAttributeId)
				return false;

			if (mAttributes[i].mAttributeIndex != aOther.mAttributes[i].mAttributeIndex)
				return false;
		}

		return true;
	}

	SC_Array<SR_VertexAttributeData> mAttributes;
};

namespace std
{
	template <>
	struct hash<SR_VertexLayout>
	{
		size_t operator()(const SR_VertexLayout& aOther) const
		{
			size_t res = 17;
			for (uint32 i = 0; i < aOther.mAttributes.Count(); ++i)
			{
				res = res * 31 + hash<uint32>()(static_cast<uint32>(aOther.mAttributes[i].mAttributeId));
				res += 9 * hash<uint32>()(static_cast<uint32>(aOther.mAttributes[i].mFormat));
				res += 24 * hash<uint32>()(static_cast<uint32>(aOther.mAttributes[i].mAttributeIndex));
			}
			return res;
		}
	};
}