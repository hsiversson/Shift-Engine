#pragma once
#include <string>

class SC_FilePath
{
public:
	static std::string GetFileName(const SC_FilePath& aPath);
	static std::string GetFileNameWithoutExtension(const SC_FilePath& aPath);
	static std::string GetFileExtension(const SC_FilePath& aPath);  // Returns file extension WITH the dot
	static std::string GetAbsolutePath(const SC_FilePath& aPath);
	static SC_FilePath GetParentDirectory(const SC_FilePath& aPath);
	static SC_FilePath Normalize(const SC_FilePath& aPath, bool aAbsolute = true);
	static bool IsRelativePath(const SC_FilePath& aPath);
	static bool IsAbsolutePath(const SC_FilePath& aPath);

	static void CreateDirectory(const SC_FilePath& aPath);

	static bool Exists(const SC_FilePath& aPath);

public:
	SC_FilePath();
	SC_FilePath(const char* aPath);
	SC_FilePath(const std::string& aPath);
	~SC_FilePath();

	std::string GetFileName() const;
	std::string GetFileExtension() const; // Returns file extension WITH the dot
	void RemoveExtension();
	std::string GetAbsolutePath() const;
	SC_FilePath GetParentDirectory() const;
	bool IsRelativePath() const;
	bool IsAbsolutePath() const;

	void MakeAbsolute();

	const char* GetStr() const;
	bool IsEmpty() const;

	void FixSlashes();

	SC_FilePath operator+(const SC_FilePath& aPath) const;
	SC_FilePath operator+(const std::string& aStr) const;
	SC_FilePath operator+(const char* aStr) const;
	SC_FilePath& operator+=(const SC_FilePath& aPath);
	SC_FilePath& operator+=(const std::string& aStr);
	SC_FilePath& operator+=(const char* aStr);

private:
	std::string mPath;
};

namespace SC_File
{
	uint64 GetFileSize(const SC_FilePath& aPath);
}


