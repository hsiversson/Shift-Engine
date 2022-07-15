#pragma once
#include <string>
#include <string_view>
#include <format>

std::wstring SC_UTF8ToUTF16(const char* aStr);
std::wstring SC_UTF8ToUTF16(const std::string& aStr);
std::string SC_UTF16ToUTF8(const wchar_t* aStr);
std::string SC_UTF16ToUTF8(const std::wstring& aStr);

void SC_ToLower(std::string& aStr);
void SC_ToLower(std::wstring& aStr);
void SC_ToUpper(std::string& aStr);
void SC_ToUpper(std::wstring& aStr);

uint32 SC_Strlen(const char* aStr);
uint32 SC_Strlen(const wchar_t* aStr);

SC_FORCEINLINE bool SC_IsUpper(char aChar)
{
	return (aChar >= 'A' && aChar <= 'Z');
}

SC_FORCEINLINE bool SC_IsLower(char aChar)
{
	return (aChar >= 'a' && aChar <= 'z');
}

SC_FORCEINLINE bool SC_IsDigit(char aChar)
{
	return (aChar >= '0' && aChar <= '9');
}

SC_FORCEINLINE bool SC_IsSpace(char aChar)
{
	unsigned char c = static_cast<unsigned char>(aChar - 1);
	if (c >= 32)
		return false;

	const uint32 mask =
		(uint32(1) << (' ' - 1)) |
		(uint32(1) << ('\t' - 1)) |
		(uint32(1) << ('\n' - 1)) |
		(uint32(1) << ('\v' - 1)) |
		(uint32(1) << ('\f' - 1)) |
		(uint32(1) << ('\r' - 1));

	return ((uint32(1) << c) & mask) != 0;
}

template<typename ... Args>
std::string SC_FormatStr(std::string_view aFormat, Args&&... aArgs)
{
	return std::vformat(aFormat, std::make_format_args(aArgs...));
}