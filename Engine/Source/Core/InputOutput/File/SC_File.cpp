#include "SC_File.h"
#include "EngineTypes/String/SC_StringHelpers.h"

#include <filesystem>

std::string SC_FilePath::GetFileName(const SC_FilePath& aPath)
{
	std::filesystem::path p(aPath.mPath.c_str());
	return SC_UTF16ToUTF8(p.filename().c_str());
}

std::string SC_FilePath::GetFileNameWithoutExtension(const SC_FilePath& aPath)
{
	std::string name = GetFileName(aPath);
	return name.substr(0, name.rfind('.'));
}

std::string SC_FilePath::GetFileExtension(const SC_FilePath& aPath)
{
	std::filesystem::path p(aPath.mPath.c_str());
	return SC_UTF16ToUTF8(p.extension().c_str());
}

std::string SC_FilePath::GetFileName() const
{
	return GetFileName(*this);
}

std::string SC_FilePath::GetFileExtension() const
{
	return GetFileExtension(*this);
}

void SC_FilePath::RemoveExtension()
{
	std::string extension = GetFileExtension();
	mPath = mPath.substr(0, mPath.rfind(extension));
}

std::string SC_FilePath::GetAbsolutePath(const SC_FilePath& aPath)
{
	std::filesystem::path p(aPath.mPath.c_str());

	std::error_code error;
	std::filesystem::path absoluteP = std::filesystem::absolute(p, error);
	if (error)
		SC_ASSERT(false, "Filesystem error: {}", error.message().c_str());

	return SC_UTF16ToUTF8(absoluteP.c_str());
}

std::string SC_FilePath::GetAbsolutePath() const
{
	return GetAbsolutePath(*this);
}

SC_FilePath SC_FilePath::GetParentDirectory() const
{
	return GetParentDirectory(*this);
}

SC_FilePath SC_FilePath::GetParentDirectory(const SC_FilePath& aPath)
{
	std::filesystem::path p(aPath.mPath.c_str());
	return SC_UTF16ToUTF8(p.parent_path().c_str()).c_str();
}

SC_FilePath SC_FilePath::Normalize(const SC_FilePath& aPath, bool aAbsolute)
{
	std::filesystem::path p(aPath.mPath.c_str());

	if (aAbsolute)
	{
		std::error_code error;
		p = std::filesystem::canonical(p, error);
		if (error)
			SC_ASSERT(false, "Filesystem error: {}", error.message().c_str());
	}
	else
	{
		std::error_code error;
		p = std::filesystem::weakly_canonical(p, error);
		if (error)
			SC_ASSERT(false, "Filesystem error: {}", error.message().c_str());
	}

	return SC_UTF16ToUTF8(p.c_str()).c_str();
}

bool SC_FilePath::IsRelativePath(const SC_FilePath& aPath)
{
	std::filesystem::path p(aPath.mPath.c_str());
	return p.is_relative();
}

bool SC_FilePath::IsRelativePath() const
{
	return IsRelativePath(*this);
}

bool SC_FilePath::IsAbsolutePath(const SC_FilePath& aPath)
{
	std::filesystem::path p(aPath.mPath.c_str());
	return p.is_absolute();
}

void SC_FilePath::CreateDirectory(const SC_FilePath& aPath)
{
	std::filesystem::path p(aPath.mPath.c_str());
	std::filesystem::create_directories(p);
}

bool SC_FilePath::Exists(const SC_FilePath& aPath)
{
	std::filesystem::path p(aPath.mPath.c_str());
	return std::filesystem::exists(p);
}

bool SC_FilePath::IsAbsolutePath() const
{
	return IsAbsolutePath(*this);
}

void SC_FilePath::MakeAbsolute()
{
	mPath = GetAbsolutePath(*this);
	FixSlashes();
}

const char* SC_FilePath::GetStr() const
{
	return mPath.c_str();
}

bool SC_FilePath::IsEmpty() const
{
	return mPath.empty();
}

void SC_FilePath::FixSlashes()
{
	std::replace(mPath.begin(), mPath.end(), '\\', '/');
}

SC_FilePath SC_FilePath::operator+(const SC_FilePath& aPath) const
{
	SC_FilePath newPath;
	newPath.mPath = mPath + aPath.mPath;
	return newPath;
}

SC_FilePath SC_FilePath::operator+(const std::string& aStr) const
{
	SC_FilePath newPath;
	newPath.mPath = mPath + aStr;
	return newPath;
}

SC_FilePath SC_FilePath::operator+(const char* aStr) const
{
	SC_FilePath newPath;
	newPath.mPath = mPath + std::string(aStr);
	return newPath;
}

SC_FilePath& SC_FilePath::operator+=(const SC_FilePath& aPath)
{
	mPath += aPath.mPath;
	return *this;
}

SC_FilePath& SC_FilePath::operator+=(const std::string& aStr)
{
	mPath += aStr;
	return *this;
}

SC_FilePath& SC_FilePath::operator+=(const char* aStr)
{
	mPath += std::string(aStr);
	return *this;
}

SC_FilePath::SC_FilePath()
{

}

SC_FilePath::SC_FilePath(const char* aPath)
	: mPath(aPath)
{
	FixSlashes();
}

SC_FilePath::SC_FilePath(const std::string& aPath)
	: mPath(aPath)
{
	FixSlashes();
}

SC_FilePath::~SC_FilePath()
{

}

uint64 SC_File::GetFileSize(const SC_FilePath& aPath)
{
	std::filesystem::path p(aPath.GetStr());
	return std::filesystem::file_size(p);
}
