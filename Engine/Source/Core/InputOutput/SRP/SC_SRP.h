#pragma once

/*
*
*	SRP (Shift Resource Package) - is a compressed resource archive containing data such as textures, meshes, shaders..
* 
*/

static constexpr uint32 gCurrentSRPVersion = 0x1;

enum class SC_SRPTypes
{
	Mesh,
	Texture,
	Shader,
};

struct SC_SRPHeader
{
	uint32 mVersion;

};

struct SC_SRPData
{
	SC_SRPHeader mHeader;
};