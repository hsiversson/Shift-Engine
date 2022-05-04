#pragma once
#include <string>
#include <format>

std::wstring SC_UTF8ToUTF16(const char* aStr);
std::wstring SC_UTF8ToUTF16(const std::string& aStr);
std::string SC_UTF16ToUTF8(const wchar_t* aStr);
std::string SC_UTF16ToUTF8(const std::wstring& aStr);

void SC_ToLower(std::string& aStr);
void SC_ToLower(std::wstring& aStr);
void SC_ToUpper(std::string& aStr);
void SC_ToUpper(std::wstring& aStr);

template<typename ... Args>
std::string SC_FormatStr(const std::string& aFormat, Args ... aArgs)
{
	return std::format(aFormat, std::forward<Args>(aArgs)...);
}