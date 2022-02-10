#pragma once
#include <string>

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
	size_t size = static_cast<size_t>(snprintf(nullptr, (size_t)0, aFormat.c_str(), aArgs ...) + (size_t)1);
	if (size <= 0)
	{
		throw std::runtime_error("Error during formatting.");
	}

	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, aFormat.c_str(), aArgs ...);
	return std::string(buf.get(), buf.get() + size - 1);
}