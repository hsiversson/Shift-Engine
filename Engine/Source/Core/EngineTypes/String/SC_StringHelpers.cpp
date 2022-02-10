#include "SC_StringHelpers.h"

#pragma warning(push)
#pragma warning(disable:4244)
#pragma warning(disable:4996)

#include <locale>
#include <codecvt>
#include <algorithm>


std::wstring SC_UTF8ToUTF16(const char* aStr)
{
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(aStr);
}

std::wstring SC_UTF8ToUTF16(const std::string& aStr)
{
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(aStr.c_str());
}

std::string SC_UTF16ToUTF8(const wchar_t* aStr)
{
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(aStr);
}

std::string SC_UTF16ToUTF8(const std::wstring& aStr)
{
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(aStr.c_str());
}

void SC_ToLower(std::string& aStr)
{
	std::transform(aStr.begin(), aStr.end(), aStr.begin(), ::tolower);
}

void SC_ToLower(std::wstring& aStr)
{
	std::transform(aStr.begin(), aStr.end(), aStr.begin(), ::towlower);
}

void SC_ToUpper(std::string& aStr)
{
	std::transform(aStr.begin(), aStr.end(), aStr.begin(), ::toupper);
}

void SC_ToUpper(std::wstring& aStr)
{
	std::transform(aStr.begin(), aStr.end(), aStr.begin(), ::towupper);
}

#pragma warning(pop)