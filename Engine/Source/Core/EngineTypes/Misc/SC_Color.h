#pragma once
#include "Platform/Types/SC_TypeDefines.h"

/*
*	Stores color values in 0..255 range using 8 bits per channel
*/
class alignas(16) SC_Color
{
public:
	uint8 r, g, b, a;

public:
	SC_Color() : r(0), g(0), b(0), a(0) {}
	SC_Color(uint8 aRed, uint8 aGreen, uint8 aBlue, uint8 aAlpha) : r(aRed), g(aGreen), b(aBlue), a(aAlpha) {}


};

/* 
*	Stores color values in 0..1 range using 32 bits per channel
*/
class alignas(16) SC_LinearColor
{
public:
	float r, g, b, a;

public:
	SC_LinearColor() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}
	SC_LinearColor(float aRed, float aGreen, float aBlue, float aAlpha) : r(aRed), g(aGreen), b(aBlue), a(aAlpha) {}
};

inline SC_LinearColor SC_ConvertColorToLinear(const SC_Color& aColor)
{
	return SC_LinearColor(aColor.r / 255.0f, aColor.g / 255.0f, aColor.b / 255.0f, aColor.a / 255.0f);
}

inline SC_Color SC_ConvertLinearToColor(const SC_LinearColor& aLinearColor)
{
	return SC_Color(uint8(aLinearColor.r * 255.0f), uint8(aLinearColor.g * 255.0f), uint8(aLinearColor.b * 255.0f), uint8(aLinearColor.a * 255.0f));
}