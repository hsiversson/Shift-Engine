#pragma once
#include "SR_BufferResource.h"

enum class SR_BufferType
{
	Default,
	Bytes,
	Structured,
	RaytracingBuffer,
	COUNT
};

struct SR_BufferProperties
{
	SR_BufferProperties() : mFirstElement(0), mElementCount(0), mFormat(SR_Format::UNKNOWN), mType(SR_BufferType::Default), mWritable(false) {}

	uint32 mFirstElement;
	uint32 mElementCount;

	SR_Format mFormat;
	SR_BufferType mType;
	bool mWritable;
};

class SR_Buffer
{
public:
	SR_Buffer(const SR_BufferProperties& aProperties, const SC_Ref<SR_BufferResource>& aResource);
	virtual ~SR_Buffer();

	const SR_Descriptor& GetDescriptor() const;
	uint32 GetDescriptorHeapIndex() const;

	const SR_BufferProperties& GetProperties() const;
	SR_BufferResource* GetResource() const;

protected:
	SR_BufferProperties mProperties;
	SC_Ref<SR_BufferResource> mResource;
	SR_Descriptor mDescriptor;
};

