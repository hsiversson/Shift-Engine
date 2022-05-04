#pragma once

/*
*
*	SRP (Shift Resource Package) - is a compressed resource archive containing data such as textures, meshes, shaders..
* 
*/

static constexpr uint32 SC_CurrentSRPVersion = 0x1;
static constexpr const char* SC_SRPHeaderFileExtension = ".srph";

enum class SC_SRPTypes
{
	Mesh,
	Texture,
	Shader,
};

struct SC_SRPHeader
{
	uint32 mFourCC;
	uint32 mVersion;
	uint32 mSize;
	uint32 mCompressedSize;


};

struct SC_SRPData
{
	SC_SRPHeader mHeader;
};